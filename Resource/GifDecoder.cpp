//
// Class to load images from GIF files
//
// Author: Alex V. Boreskoff
//
// Last modified: 6/11/2002
//

#include	"GifDecoder.h"
#include	"ObjStr.h"
#include	"Data.h"
#include	"ResourceManager.h"
#include	"Texture.h"

#define MAX_CODES   4095

#pragma pack (push, 1)
										// GIF specific constants and structs
struct	GifHeader
{
	char	sign [6];					// "GIF87a" or "GIF89a"
										// Screen descriptor
	short	screenWidth;
	short	screenHeight;
	char	misc;
	char	back;						// background color;
	char	null;						// always '\0'
};

struct	GifImageHeader
{
	char	sign;						// '\x2C' == ','
	short	x, y;						// top left corner
	short	width, height;
	char	misc;
};

#pragma	pack (pop)

/////////////////////////// Static data /////////////////////////////

static	int	codeSize;					// The current code size
static	int	clearCode;					// Value for a clearCode code
static	int	endingCode;					// Value for a endingCode code
static	int	freeCode;					// First available code
static	int	topSlot;					// Highest code for current size
static	int	slot;              			// Last read code

static	unsigned char	b1;             // Current byte
static	unsigned char	buf [257];      // Current block
static	unsigned char  * pbytes;        // Pointer to next byte in block
static	int		bytesInBlock  = 0;      // # bytes left in block
static	int		bitsLeft      = 0;      // # bits left in current byte
static	long	codeMask [13] =			// bit masks for codes of var. lengths
{
     0,
     0x0001, 0x0003,
     0x0007, 0x000F,
     0x001F, 0x003F,
     0x007F, 0x00FF,
     0x01FF, 0x03FF,
     0x07FF, 0x0FFF
};

static	unsigned char	stack  [MAX_CODES + 1]; // Stack for storing pixels
static	unsigned char	suffix [MAX_CODES + 1]; // Suffix table
static	int				prefix [MAX_CODES + 1]; // Prefix linked list

MetaClass	GifDecoder :: classInstance ( "GifDecoder", &ResourceDecoder :: classInstance );

bool	GifDecoder :: checkExtension ( const String& theName )
{
	String	ext ( String :: getExtension ( theName ).toLower () );

	return ext == "gif";
}

Object * GifDecoder :: decode ( Data * data )
{
	PixelFormat	rgbFormat  ( getResourceManager () -> getRgbFormat  () );
	PixelFormat	rgbaFormat ( getResourceManager () -> getRgbaFormat () );

	GifHeader	hdr;

	if ( data -> getBytes ( &hdr, sizeof ( hdr ) ) != sizeof ( hdr ) )
		return NULL;

	if ( strncmp ( hdr.sign, "GIF87a", 6 ) && strncmp ( hdr.sign, "GIF89a", 6 ) )
		return NULL;

	int	  transpColor  = -1;	// index of transparent color
	int	  bitsPerPixel = 1 + ( 7 & hdr.misc );
	Rgb * pal          = NULL;

	if ( hdr.misc & 0x80 )		// has global color map
	{
		pal = new Rgb [1 << bitsPerPixel];

		data -> getBytes ( pal, sizeof ( Rgb ) * (1 << bitsPerPixel) );
	}

	GifImageHeader	imageHdr;

	if ( data -> getBytes ( &imageHdr, sizeof ( imageHdr ) ) != sizeof ( imageHdr ) )
		return NULL;

	while ( imageHdr.sign == '!' )		// read gif extension blocks
	{									// and do nothing with it
										// find type & size of extension block
		int	extType = * ( 1 + (char *) &imageHdr );
		int	size    = * ( 2 + (char *) &imageHdr );

		if ( extType == 0xF9 )			// graphics ext, retrieve transparency info
		{
			if ( *( 3 + (char *) &imageHdr ) & 0x01 )
			{							// transparent image
				transpColor = * (6 + (char *) &imageHdr );
			}
		}
		
		data -> seekCur ( size + 4 - sizeof ( imageHdr ) );
		data -> getBytes ( &imageHdr, sizeof ( imageHdr ) );
	}

	if ( imageHdr.sign != ',' )
		return NULL;

	if ( imageHdr.misc & 0x80 )				// use local color map, instead of global
	{
		bitsPerPixel = 1 + ( imageHdr.misc & 7 );

		delete pal;

		pal = new Rgb [1 << bitsPerPixel];

		data -> getBytes ( pal, sizeof ( Rgb ) * (1 << bitsPerPixel ) );
	}

	int	width   = imageHdr.width;
	int	height  = imageHdr.height;
	int	size;

			   // initialize for decoding a new image...
	if ( ( size = data -> getByte () ) == -1 )
		return NULL;

	if ( size < 2 || 9 < size )
		return NULL;

	unsigned char * ptr    = new unsigned char [width * height];
	unsigned char * tmpBuf = ptr;

	codeSize     = size + 1;
	topSlot      = 1 << codeSize;
	clearCode    = 1 << size;
	endingCode   = clearCode + 1;
	slot         =
	freeCode     = endingCode + 1;
	bytesInBlock = bitsLeft = 0;

   // Initialize in case they forgot to put in a clearCode code.
   // (This shouldn't happen, but we'll try and decode it anyway...)

	unsigned char  * sp;
	int	    code;
	int	c;
	int	oc = 0;
	int	fc = 0;

   // Set up the stack pointer and decode buffer pointer

	sp = stack;

   // This is the main loop.  For each code we get we pass through the
   // linked list of prefix codes, pushing the corresponding "character" for
   // each code onto the stack.  When the list reaches a single "character"
   // we push that on the stack too, and then start unstacking each
   // character for output in the correct order.  Special handling is
   // included for the clearCode code, and the whole thing ends when we get
   // an endingCode code.

	while ( ( c = getCode ( data ) ) != endingCode )
	{
		// If we had a file error, return without completing the decode
		if ( c < 0 )
		{
			delete pal;
			delete tmpBuf;

			return NULL;
		}
	      // If the code is a clearCode code, reinitialize all necessary items.

		if ( c == clearCode )
		{
			codeSize = size + 1;
			slot     = freeCode;
			topSlot = 1 << codeSize;

	 // Continue reading codes until we get a non-clearCode code

			while ( ( c = getCode ( data ) ) == clearCode )
				;

			if ( c == endingCode )
				break;

	 // Finally, if the code is beyond the range of already set codes,
	 // then set it to color zero.

			if ( c >= slot )
				c = 0;

			oc = fc = c;

			*ptr++ = c;		// store pixel
		}
		else
		{

	 // In this case, it's not a clearCode code or an endingCode code, so
	 // it must be a code code...  So we can now decode the code into
	 // a stack of character codes. (Clear as mud, right?)

			code = c;

			if ( code >= slot )	// check for bad code
			{
				if ( code > slot )
				{
					delete pal;
					delete tmpBuf;

					return NULL;
				}

				code  = oc;
				*sp++ =fc;
			}

	 // Here we scan back along the linked list of prefixes, pushing
	 // helpless characters (ie. suffixes) onto the stack as we do so.

			while ( code >= freeCode )
			{
				*sp++ = suffix [code];
				code  = prefix [code];
			}

	 // Push the last character on the stack, and set up the new
	 // prefix and su+ffix, and if the required slot number is greater
	 // than that allowed by the current bit size, increase the bit
	 // size.  (NOTE - If we are all full, we *don't* save the new
	 // suffix and prefix...

			*sp++ = code;

			if ( slot < topSlot )
			{
				suffix [slot] = fc = code;
				prefix [slot++] = oc;
				oc = c;
			}

			if ( slot >= topSlot )
				if ( codeSize < 12 )
				{
					topSlot <<= 1;
					codeSize++;
				}

	 // Now that we've pushed the decoded string (in reverse order)
	 // onto the stack, lets pop it off and put it into our decode
	 // buffer...

			while ( sp > stack )
				*ptr++ = *--sp;
		}
	}

	Texture	* texture = new Texture ( "", width, height, transpColor == -1 ? rgbFormat : rgbaFormat );
	long	* lineBuf = new long [width];

	for ( int i = 0; i < height; i++ )
	{
		unsigned char * p = tmpBuf + i * width;

		if ( transpColor == -1 )		// no transparency
		{
			for ( int j = 0; j < width; j++, p++ )
				lineBuf [j] = rgbToInt ( pal [*p].blue, pal [*p].green, pal [*p].red );
		}
		else
		{
			for ( int j = 0; j < width; j++, p++ )
				if ( *p == transpColor )
					lineBuf [j] = rgbaToInt ( 0, 0, 0, 0xFF );
				else
					lineBuf [j] = rgbToInt ( pal [*p].blue, pal [*p].green, pal [*p].red );
		}

		texture -> writeLine ( i, lineBuf );
	}

	delete lineBuf;
	delete tmpBuf;
	delete pal;

	return texture;
}


int	GifDecoder :: loadBlock ( Data * data )
{
	pbytes = buf;

	if ( ( bytesInBlock = data -> getByte () ) == -1 )
		return bytesInBlock;
	else
	if ( bytesInBlock > 0 )
		if ( data -> getBytes ( buf, bytesInBlock ) != bytesInBlock )
			return -1;

	return bytesInBlock;
}

//
// return next code from GIF file or EOF in case of file error
//
int	GifDecoder :: getCode ( Data * data )
{
	if ( bitsLeft == 0 )
	{
		if ( bytesInBlock <= 0 )	// read next block
			if ( loadBlock ( data ) == -1 )
				return -1;

		b1       = *pbytes++;
		bitsLeft = 8;
		bytesInBlock--;
	}

	long	ret = b1 >> ( 8 - bitsLeft );

	while ( codeSize > bitsLeft )
	{
		if ( bytesInBlock <= 0 )	// Out of bytes in current block, so read next block
			if ( loadBlock ( data ) == -1 )
				return -1;

		b1        = *pbytes++;
		ret      |= b1 << bitsLeft;
		bitsLeft += 8;
		bytesInBlock--;
	}

	bitsLeft -= codeSize;
	ret      &= codeMask [codeSize];

	return (int) ret;
}
