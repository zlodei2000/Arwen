//
// Decoder class for QuakeII bps files
//
// Author: Alex V. Boreskoff
//

#include	"BspDecoder.h"
#include	"QuakeII.h"
#include	"Quake2Level.h"
#include	"ObjStr.h"
#include	"Data.h"

MetaClass	BspDecoder :: classInstance ( "BspDecoder", &ResourceDecoder :: classInstance );

bool	BspDecoder :: checkExtension ( const String& theName )
{
	return String :: getExtension ( theName ).toLower () == "bsp";
}

Object * BspDecoder :: decode ( Data * data )
{
	Quake2BspHeader	hdr;
	
	if ( data -> getBytes ( &hdr, sizeof ( hdr ) ) != sizeof ( hdr ) )
		return false;

	if ( strncmp ( hdr.magic, "IBSP", 4 ) != 0 )
		return false;

	if ( hdr.version != BSP_VERSION )
		return false;

	Quake2Level * level = new Quake2Level ( data );

	if ( level -> isOk () )
		return level;

	level -> release ();

	return false;
}
