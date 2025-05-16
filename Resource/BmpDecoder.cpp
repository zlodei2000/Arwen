//
// Class to load images from BMP files
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#include	"BmpDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"ResourceManager.h"
#include	"Texture.h"
										// BMP specific constants and structs
#undef	BI_RGB
#undef	BI_RLE8
#undef	BI_RLE4
										
#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2
 
#pragma pack (push, 1)					// save current pack, set 1-byte packing

										// structs of BMP file
struct  BmpHeader
{
        short   type;                   // type of file, must be 'BM'
        long    size;                   // size of file in bytes
        short   reserved1, reserved2;
        long    offBits;                // offset from this header to actual data
};

struct  BmpInfoHeader
{
        long    size;                   // sizeof of this header
        long    width;                  // width of bitmap in pixels
        long    height;                 // height of bitmap in pixels
        short   planes;                 // # of planes
        short   bitCount;               // bits per pixel
        long    compression;            // type of compression, BI_RGB - no compression
        long    sizeImage;              // size of image in bytes
        long    xPelsPerMeter;          // hor. resolution of the target device
        long    yPelsPerMeter;          // vert. resolution
        long    clrUsed;
        long    clrImportant;
};

struct  RGBQuad
{
        unsigned char    blue;
        unsigned char    green;
        unsigned char    red;
        unsigned char    reserved;
};

#pragma	pack (pop)

////////////////////////// here goes methods ///////////////////////////////

#define	LE_OK			0
#define	LE_DATAERROR	1

MetaClass	BmpDecoder :: classInstance ( "BmpDecoder", &ResourceDecoder :: classInstance );

bool  BmpDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "bmp";
}

Object * BmpDecoder :: decode ( Data * data )
{
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );
	PixelFormat	rgbaFormat ( getResourceManager () -> getRgbaFormat () );

	BmpHeader     * hdr     = (BmpHeader *)     data -> getPtr ();
	BmpInfoHeader * infoHdr = (BmpInfoHeader *) data -> getPtr ( sizeof ( BmpHeader ) );

    int	errorCode = hdr -> type == 0x4D42 && infoHdr -> size == 40 && infoHdr -> planes == 1 &&
                   (infoHdr -> bitCount == 1 || infoHdr -> bitCount == 2 ||
                    infoHdr -> bitCount == 4 || infoHdr -> bitCount == 8 || infoHdr -> bitCount == 24 || infoHdr -> bitCount == 32) &&
                    (infoHdr -> compression == BI_RGB || infoHdr -> compression == BI_RLE4 || infoHdr -> compression == BI_RLE8);

	if ( !errorCode )                    // bad header buf
        return NULL;

    int	numColors = 1 << infoHdr -> bitCount;

    width  = infoHdr -> width;
    height = infoHdr -> height;

    RGBQuad * pal = (RGBQuad *)(infoHdr + 1);

	if ( infoHdr -> bitCount <= 8 )
		for ( int i = 0; i < numColors; i++ )
			palette [i] = rgbFormat.rgbaToInt ( pal [i].red, pal [i].green, pal [i].blue );
 
	Texture * texture = new Texture ( name, width, height, infoHdr -> bitCount != 32 ? rgbFormat : rgbaFormat );

    if ( texture == NULL || !texture -> isOk () )
		return NULL;

	long * pixelBuf = new long [width];

	data -> seekAbs ( hdr -> offBits );

    if ( infoHdr -> compression == BI_RGB )
    {
        if ( infoHdr -> bitCount == 4 )		// 16-colors uncompressed
			errorCode = loadRGB4 ( data, texture, pixelBuf );
         else
         if ( infoHdr -> bitCount == 8 )	// 256-colors uncompressed
            errorCode = loadRGB8 ( data, texture, pixelBuf );
         else
         if ( infoHdr -> bitCount == 24 )	// True-Color bitmap
            errorCode = loadRGB24 ( data, texture, pixelBuf );
		 else
		 if ( infoHdr -> bitCount == 32 )	// true-color bitmap with alpha-channel
			 errorCode = loadRGB32 ( data, texture, pixelBuf );
    }
    else
    if ( infoHdr -> compression == BI_RLE4 )// 16-colors RLE compressed
        errorCode = loadRLE4 ( data, texture, pixelBuf );
    else
    if ( infoHdr -> compression == BI_RLE8 )// 256-colors RLE compressed
        errorCode = loadRLE8 ( data, texture, pixelBuf );
    else
		errorCode = LE_DATAERROR;

	delete pixelBuf;

    if ( errorCode != LE_OK )
    {
        delete texture;

        return NULL;
    }

    return texture;
}

int	BmpDecoder :: loadRGB4 ( Data * data, Texture * texture, long * buf )
{
	int	count, byte;

    for ( int y = height - 1; y >= 0; y-- )
    {
		memset ( buf, '\0', sizeof ( long ) * width );

		for ( int x = 0; x < width; x++ )
			if ( ( x & 1 ) == 0 )
				if ( ( byte = data -> getByte () ) == -1 )
					return LE_DATAERROR;
                else
                    buf [x] = palette [byte >> 4];
            else
                buf [x] = palette [byte & 0x0F];

        for ( count = ( width + 1 ) / 2; count % 4; count++ )
			if ( data -> getByte () == -1 )
				return LE_DATAERROR;

		texture -> writeLine ( y, buf );
    }

    return LE_OK;
}

int	BmpDecoder :: loadRGB8 ( Data * data, Texture * texture, long * buf )
{
	int	count, byte;

    for ( int y = height - 1; y >= 0; y-- )
    {
		memset ( buf, '\0', sizeof ( long ) * width );

		for ( int x = count = 0; x < width; x++, count++ )
			if ( ( byte = data -> getByte () ) == -1 )
				return LE_DATAERROR;
            else
                buf [x] = palette [byte];

        for ( ; count % 4; count++ )    // skip remaining bytes
			if ( data -> getByte () == -1 )
				return LE_DATAERROR;

		texture -> writeLine ( y, buf );
    }

    return LE_OK;
}

int	BmpDecoder :: loadRGB24 ( Data * data, Texture * texture, long * buf )
{
	int	count;
    int	red, green, blue;

    for ( int y = height - 1; y >= 0; y-- )
    {
		memset ( buf, '\0', sizeof ( long ) * width );

		for ( int x = count = 0; x < width; x++, count += 3 )
        {
			if ( ( red = data -> getByte () ) == -1 )
				return LE_DATAERROR;

            if ( ( green = data -> getByte () ) == -1 )
                return LE_DATAERROR;

            if ( ( blue = data -> getByte () ) == -1 )
                return LE_DATAERROR;

            buf [x] = rgbToInt ( red, green, blue );
        }

        for ( ; count % 4; count++ )    // skip remaining bytes
			if ( data -> getByte () == -1 )
				return LE_DATAERROR;

		texture -> writeLine ( y, buf );
    }

    return LE_OK;
}

int	BmpDecoder :: loadRGB32 ( Data * data, Texture * texture, long * buf )
{
    int	red, green, blue, alpha;

    for ( int y = height - 1; y >= 0; y-- )
    {
		memset ( buf, '\0', sizeof ( long ) * width );

		for ( int x = 0; x < width; x++ )
        {
			if ( ( red = data -> getByte () ) == -1 )
				return LE_DATAERROR;

            if ( ( green = data -> getByte () ) == -1 )
                return LE_DATAERROR;

            if ( ( blue = data -> getByte () ) == -1 )
                return LE_DATAERROR;

			if ( ( alpha = data -> getByte () ) == -1 )
				return LE_DATAERROR;

            buf [x] = rgbaToInt ( red, green, blue, alpha );
        }

		texture -> writeLine ( y, buf );
    }

    return LE_OK;
}

int	BmpDecoder :: loadRLE4 ( Data * data, Texture * texture, long * buf )
{
	int	y = height - 1;
    int x = 0;
    int count, byte;

	memset ( buf, '\0', sizeof ( long ) * width );

    for ( ; ; )
    {
		if ( ( count = data -> getByte () ) == -1 )
			return LE_DATAERROR;
        else
        if ( count == 0 )
        {
			if ( ( count = data -> getByte () ) == -1 )
				return LE_DATAERROR;
            else
            if ( count == 0 )       // end of line
            {
				texture -> writeLine ( y, buf );

				memset ( buf, '\0', sizeof ( long ) * width );

				y--;
                x = 0;
            }
            else
            if ( count == 1 )       // 0, 1 == end of RLE buf
				break;
            else
            if ( count == 2 )       // 0, 2 == delta record
            {
				texture -> writeLine ( y, buf );

				memset ( buf, '\0', sizeof ( long ) * width );

				y -= data -> getByte ();
                x += data -> getByte ();
            }
            else                    // start of an unencoded block
            {
				for ( int i = 0; i < count; i += 2, x += 2 )
                {
					if ( ( byte = data -> getByte () ) == -1 )
						return LE_DATAERROR;

                    buf [x]     = palette [byte >> 4];
                    buf [x + 1] = palette [byte & 0x0F];
                }

                if ( ( count / 2 ) & 1 )
					if ( data -> getByte () == -1 )
						return LE_DATAERROR;
            }
        }
        else                    // RLE decoded record
        {
			if ( ( byte = data -> getByte () ) == -1 )
				return LE_DATAERROR;

            for ( int i = 0; i < count; i++, x++ )
				if ( i & 1 )
					buf [x] = palette [byte & 0x0F];
                else
					buf [x] = palette [byte >> 4];
        }
    }

    return LE_OK;
}

int	BmpDecoder :: loadRLE8 ( Data * data, Texture * texture, long * buf )
{
	int	y = height - 1;
    int x = 0;
	int count, byte;

	memset ( buf, '\0', sizeof ( long ) * width );

    for ( ; ; )
    {
		if ( ( count = data -> getByte () ) == -1 )
			return LE_DATAERROR;
        else
        if ( count == 0 )
        {
			if ( ( count = data -> getByte () ) == -1 )
				return LE_DATAERROR;
            else
            if ( count == 0 )       // end of line
            {
				texture -> writeLine ( y, buf );

				memset ( buf, '\0', sizeof ( long ) * width );

				y--;
                x = 0;
            }
            else
            if ( count == 1 )       // 0, 1 == end of RLE buf
				break;
            else
            if ( count == 2 )       // 0, 2 == delta record
            {
				texture -> writeLine ( y, buf );

				memset ( buf, '\0', sizeof ( long ) * width );

				y -= data -> getByte ();
                x += data -> getByte ();
            }
            else                    // start of an unencoded block
            {
				for ( int i = 0; i < count; i++, x ++ )
					if ( ( byte = data -> getByte () ) == -1 )
						return LE_DATAERROR;
                    else
						buf [x] = palette [byte];

                if ( count & 1 )
					if ( data -> getByte () == -1 )
						return LE_DATAERROR;
            }
        }
        else                    // RLE decoded record
        {
			if ( ( byte = data -> getByte () ) == -1 )
				return LE_DATAERROR;

            for ( int i = 0; i < count; i++, x++ )
				buf [x] = palette [byte];
        }
	}

    return LE_OK;
}
