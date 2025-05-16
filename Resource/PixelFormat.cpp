#include	"PixelFormat.h"

void    PixelFormat :: completeFromMasks ( unsigned theRedMask, unsigned theGreenMask, unsigned theBlueMask, unsigned theAlphaMask )
{
    register unsigned int s;

	redMask   = theRedMask;
	greenMask = theGreenMask;
	blueMask  = theBlueMask;
	alphaMask = theAlphaMask;

    redShift = 0;
    redBits  = 0;

    for ( s = redMask; s && !(s&1); s >>= 1 )
        redShift++;

    for ( s = redMask >> redShift; s; s >>= 1 )
        redBits++;

    greenShift = 0;
    greenBits  = 0;

    for ( s = greenMask; s && !(s&1); s >>= 1 )
        greenShift++;


    for ( s = greenMask >> greenShift; s; s >>= 1 )
        greenBits++;

    blueShift = 0;
    blueBits  = 0;

    for ( s = blueMask; s && !(s&1); s >>= 1 )
         blueShift++;

    for ( s = blueMask >> blueShift; s; s >>= 1 )
		blueBits++;

    alphaShift = 0;
    alphaBits  = 0;

    for ( s = alphaMask; s && !(s&1); s >>= 1 )
        alphaShift++;

    for ( s = alphaMask >> alphaShift; s; s >>= 1 )
		alphaBits++;

    bitsPerPixel  = redBits + greenBits + blueBits + alphaBits;
	bytesPerPixel = (bitsPerPixel + 7) >> 3;
}
