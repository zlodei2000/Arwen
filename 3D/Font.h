#ifndef __FONT__
#define __FONT__

class	View;
class	Texture;

#include	"Vector4D.h"

class Font
{
private:
	Texture * fontImage;				// texture with font image
	int       charWidth;				// character width
	int       charHeight;				// character height
	int       spacing;					// character spacing
	int		  charsPerRow;				// # of characters per image row
	Vector4D  color;
public:
	Font ( Texture * image, int cWidth, int cHeight, int spc, int cPerRow );
	~Font();

	void	setColor ( const Vector4D& theColor )
	{
		color = theColor;
	}

	void print ( View * view, int x, int y, const char * string, ... );
};

#endif