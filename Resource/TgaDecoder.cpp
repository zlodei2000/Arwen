//
// Class to load images from TGA files
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#include	"TgaDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"ResourceManager.h"
#include	"Texture.h"
#include	"TgaStructs.h"

#define	LE_OK			0
#define	LE_DATAERROR	1

MetaClass	TgaDecoder :: classInstance ( "TgaDecoder", &ResourceDecoder :: classInstance );

bool	TgaDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "tga";
}

Object * TgaDecoder :: decode ( Data * data )
{
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );
	PixelFormat	rgbaFormat ( getResourceManager () -> getRgbaFormat () );
	PixelFormat monoFormat ( getResourceManager () -> getMonoFormat () );

	hdr = (TgaHeader *) data -> getPtr ();

	if ( hdr -> imageType != 0 && hdr -> imageType != 1 && hdr -> imageType != 2 && 
		 hdr -> imageType != 3 && hdr -> imageType != 9	&& hdr -> imageType != 10 && 
		 hdr -> imageType != 11 )                    // unsupported types
        return NULL;

//	if ( hdr -> imageType == 1 || hdr -> imageType == 9 )
//		if ( hdr -> colormapType != 1 || (hdr -> colormapEntrySize != 24 && hdr -> colormapEntrySize != 32) )
//			return NULL;

	if ( hdr -> pixelSize != 32 && hdr -> pixelSize != 24 && hdr -> pixelSize != 16 && hdr -> pixelSize != 15 && hdr -> pixelSize != 8 )
		return NULL;

    width  = hdr -> width;
    height = hdr -> height;

	int	offs   = sizeof ( TgaHeader );

	if ( hdr -> idLength != 0 )
		offs += hdr -> idLength;

	unsigned char * bits = offs + (unsigned char *) data -> getPtr ();

	data -> seekAbs ( offs );

	PixelFormat	format;

	if ( hdr -> pixelSize == 8 )				// grey-scale or paletted texture
	{
		if ( hdr -> colormapEntrySize == 24 )
			format = rgbFormat;					// RGB texture
		else
		if ( hdr -> colormapEntrySize == 32 )	// RGBA texture
			format = rgbaFormat;
		else
			format = monoFormat;				// grey-scale texture
	}
	else
	if ( hdr -> pixelSize == 24 )				// true-color (RGB) texture
		format = rgbFormat;
	else
		format = rgbaFormat;					// 32-bit ( RGBA) texture

    Texture * texture = new Texture ( name, width, height, format );

    if ( texture == NULL )
		return NULL;

    if ( !texture -> isOk () )
    {
		delete texture;

        return NULL;
    }

	long * pixelBuf = new long [width];

	if ( hdr -> imageType == 1 || hdr -> imageType == 9 )		// color-mapped image
	{
		palette = new long [hdr -> colormapLength];

		if ( hdr -> colormapEntrySize == 15 || hdr -> colormapEntrySize == 16 )
		{
			unsigned char * ptr = (unsigned char *) bits;
			int				a, b;

			for ( int i = 0; i < hdr -> colormapLength; i++ )
			{
				a = *ptr++;
				b = *ptr++;

				palette [i] = rgbToInt ( (b & 0x7C) >> 2, 
				                         ((b & 0x03) << 3) | ((a & 0xE0) >> 5), 
								         a & 0x1F );
			}

			bits += 2 * hdr -> colormapLength;

			data -> seekCur ( 2 * hdr -> colormapLength );
		}
		else
		if ( hdr -> colormapEntrySize == 24 )
		{
			Rgb * pal = (Rgb *)bits;

			for ( int i = 0; i < hdr -> colormapLength; i++, pal++ )
				palette [i] = rgbToInt ( pal -> red, pal -> green, pal -> blue );

			bits += 3 * hdr -> colormapLength;

			data -> seekCur ( 3 * hdr -> colormapLength );
		}
		else
		if ( hdr -> colormapEntrySize == 32 )
		{
			memcpy ( palette, bits, sizeof ( long ) * hdr -> colormapLength );

			bits += 4 * hdr -> colormapLength;

			data -> seekCur ( 4 * hdr -> colormapLength );
		}
	}
	else
		palette = NULL;

	int	errorCode;

    if ( hdr -> imageType >= 1 && hdr -> imageType <= 3 )		// unencoded image
	{
		if ( hdr -> pixelSize == 8 )
			errorCode = loadRGB8  ( data, texture, pixelBuf );
		else
		if ( hdr -> pixelSize == 24 )
			errorCode = loadRGB24 ( data, texture, pixelBuf );
		else
			errorCode = loadRGB32 ( data, texture, pixelBuf );
	}
	else
	if ( hdr -> imageType >= 9 && hdr -> imageType <= 11 )		// RLE image
	{
		if ( hdr -> pixelSize == 8 )
			errorCode = loadRLE8  ( data, texture, pixelBuf );
		else
		if ( hdr -> pixelSize == 24 )
			errorCode = loadRLE24 ( data, texture, pixelBuf );
		else
			errorCode = loadRLE32 ( data, texture, pixelBuf );
	}

	delete palette;
	delete pixelBuf;

    return texture;
}

int	TgaDecoder :: loadRGB8 ( Data * data, Texture * texture, long * buf )
{
	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			int	val = data -> getByte ();

			if ( val == -1 )
				return LE_DATAERROR;

			buf [x] = val;
		}

		if ( hdr -> imageDesc & 0x20 )
			texture -> writeLine ( y, buf, palette );
		else
			texture -> writeLine ( height - 1 - y, buf, palette );
	}

	return LE_OK;
}

int	TgaDecoder :: loadRGB15 ( Data * data, Texture * texture, long * buf )
{
	int a, b;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			a = data -> getByte ();
			b = data -> getByte ();

			if ( a == -1 || b == -1 )
				return LE_DATAERROR;

			buf [x] = rgbToInt ( (b & 0x7C) >> 2, 
				                 ((b & 0x03) << 3) | ((a & 0xE0) >> 5), 
								 a & 0x1F );
		}

		if ( hdr -> imageDesc & 0x20 )
			texture -> writeLine ( y, buf );
		else
			texture -> writeLine ( height - 1 - y, buf );
	}

	return LE_OK;
}

int	TgaDecoder :: loadRGB24 ( Data * data, Texture * texture, long * buf )
{
	int red, green, blue;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			red   = data -> getByte ();
			green = data -> getByte ();
			blue  = data -> getByte ();

			if ( blue == -1 || green == -1 || blue == -1 )
				return LE_DATAERROR;

			buf [x] = rgbToInt ( red, green, blue );
		}

		if ( hdr -> imageDesc & 0x20 )
			texture -> writeLine ( y, buf );
		else
			texture -> writeLine ( height - 1 - y, buf );
	}

	return LE_OK;
}

int	TgaDecoder :: loadRGB32 ( Data * data, Texture * texture, long * buf )
{
	int red, green, blue, alpha;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			red   = data -> getByte ();
			green = data -> getByte ();
			blue  = data -> getByte ();
			alpha = data -> getByte ();

			if ( blue == -1 || green == -1 || blue == -1 || alpha == -1 )
				return LE_DATAERROR;

			buf [x] = rgbaToInt ( red, green, blue, alpha );
		}

		if ( hdr -> imageDesc & 0x20 )
			texture -> writeLine ( y, buf );
		else
			texture -> writeLine ( height - 1 - y, buf );
	}

	return LE_OK;
}

int	TgaDecoder :: loadRLE8 ( Data * data, Texture * texture, long * buf )
{
	int	val;
	int	packetHeader, packetSize;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x )
		{
			packetHeader = data -> getByte ();
			packetSize   = (packetHeader & 0x7F) + 1;

			if ( packetHeader == -1 )
				return LE_DATAERROR;

			if ( packetHeader & 0x80 )				// run-length packet
			{
				val = data -> getByte ();

				if ( val == -1 )
					return LE_DATAERROR;

				for ( int c = 0; c < packetSize; c++ )
				{
					buf [x++] = val;

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf, palette );
						else
							texture -> writeLine ( height - 1 - y, buf, palette );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
			else									// non run-length packet
			{
				for ( int i = 0; i < packetSize; i++ )
				{
					val = data -> getByte ();

					if ( val == -1 )
						return LE_DATAERROR;

					buf [x++] = val;

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf, palette );
						else
							texture -> writeLine ( height - 1 - y, buf, palette );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
		
		}
	}

	return LE_OK;
}

int	TgaDecoder :: loadRLE15 ( Data * data, Texture * texture, long * buf )
{
	int	a, b;
	int	red, green, blue;
	int	packetHeader, packetSize;
	int	color;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x )
		{
			packetHeader = data -> getByte ();
			packetSize   = (packetHeader & 0x7F) + 1;

			if ( packetHeader == -1 )
				return LE_DATAERROR;

			if ( packetHeader & 0x80 )				// run-length packet
			{
				a = data -> getByte ();
				b = data -> getByte ();

				if ( a == -1 || b == -1 )
					return LE_DATAERROR;

				red   = (b & 0x7C) >> 2;
				green = ((b & 0x03) << 3) + ((a & 0xE0) >> 5);
				blue  = a & 0x1F;

				color = rgbToInt ( red, green, blue );

				for ( int c = 0; c < packetSize; c++ )
				{
					buf [x++] = color;

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf );
						else
							texture -> writeLine ( height - 1 - y, buf );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
			else									// non run-length packet
			{
				for ( int i = 0; i < packetSize; i++ )
				{
					red   = data -> getByte ();
					green = data -> getByte ();
					blue  = data -> getByte ();

					if ( red == -1 || green == -1 || blue == -1 )
						return LE_DATAERROR;

					buf [x++] = rgbToInt ( red, green, blue );

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf );
						else
							texture -> writeLine ( height - 1 - y, buf );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
		
		}
	}

	return LE_OK;
}

int	TgaDecoder :: loadRLE24 ( Data * data, Texture * texture, long * buf )
{
	int	red, green, blue;
	int	packetHeader, packetSize;
	int	color;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x )
		{
			packetHeader = data -> getByte ();
			packetSize   = (packetHeader & 0x7F) + 1;

			if ( packetHeader == -1 )
				return LE_DATAERROR;

			if ( packetHeader & 0x80 )				// run-length packet
			{
				red   = data -> getByte ();
				green = data -> getByte ();
				blue  = data -> getByte ();

				if ( red == -1 || green == -1 || blue == -1 )
					return LE_DATAERROR;

				color = rgbToInt ( red, green, blue );

				for ( int c = 0; c < packetSize; c++ )
				{
					buf [x++] = color;

					if ( x >= width )				// wrapped on next scan line
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf );
						else
							texture -> writeLine ( height - 1 - y, buf );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
			else									// non run-length packet
			{
				for ( int i = 0; i < packetSize; i++ )
				{
					red   = data -> getByte ();
					green = data -> getByte ();
					blue  = data -> getByte ();

					if ( red == -1 || green == -1 || blue == -1 )
						return LE_DATAERROR;

					buf [x++] = rgbToInt ( red, green, blue );

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf );
						else
							texture -> writeLine ( height - 1 - y, buf );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
		
		}
	}

	return LE_OK;
}

int	TgaDecoder :: loadRLE32 ( Data * data, Texture * texture, long * buf )
{
	int	red, green, blue, alpha;
	int	packetHeader, packetSize;
	int	color;

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x )
		{
			packetHeader = data -> getByte ();
			packetSize   = packetHeader & 0x7F;

			if ( packetHeader == -1 )
				return LE_DATAERROR;

			if ( packetHeader & 0x80 )				// run-length packet
			{
				red   = data -> getByte ();
				green = data -> getByte ();
				blue  = data -> getByte ();
				alpha = data -> getByte ();

				if ( red == -1 || green == -1 || blue == -1 || alpha == -1 )
					return LE_DATAERROR;

				color = rgbaToInt ( red, green, blue, alpha );

				for ( int c = 0; c < packetSize; c++ )
				{
					buf [x++] = color;

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf );
						else
							texture -> writeLine ( height - 1 - y, buf );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
			else									// non run-length packet
			{
				for ( int i = 0; i < packetSize; i++ )
				{
					red   = data -> getByte ();
					green = data -> getByte ();
					blue  = data -> getByte ();
					alpha = data -> getByte ();

					if ( red == -1 || green == -1 || blue == -1 || alpha == -1 )
						return LE_DATAERROR;

					buf [x++] = rgbaToInt ( red, green, blue, alpha );

					if ( x >= width )
					{
						if ( hdr -> imageDesc & 0x20 )
							texture -> writeLine ( y, buf );
						else
							texture -> writeLine ( height - 1 - y, buf );

						memset ( buf, '\0', width * sizeof ( long ) );

						x = 0;

						if ( ++y >= height )
							return LE_OK;
					}
				}
			}
		
		}
	}

	return LE_OK;
}
