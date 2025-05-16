//
// Basic structs specific to tga file format
//
// Author: Alex V. Boreskoff
//
// Last modified: 20/11/2002
//

#ifndef	__TGA_STRUCTS__
#define	__TGA_STRUCTS__

#include	"typedefs.h"

#pragma pack (push, 1)
										// TGA specific constants and structs
struct  TgaHeader
{
	byte	idLength;
	byte	colormapType;
	byte	imageType;
	uint16	colormapIndex;
	uint16	colormapLength;
	byte	colormapEntrySize;
	uint16	xOrigin;
	uint16	yOrigin;
	uint16	width;
	uint16	height;
	byte	pixelSize;
	byte	imageDesc;
};

#pragma	pack (pop)


#endif
