
#include	<windows.h>
#include	<gl\gl.h>
#include	<stdio.h>

#include	"Font.h"
#include	"Texture.h"
#include	"OpenGLView.h"

Font :: Font ( Texture * image, int cWidth, int cHeight, int spc, int cPerRow )
{
	fontImage   = (Texture *) image -> retain ();;
	charWidth   = cWidth;
	charHeight  = cHeight;
	spacing     = spc;
	charsPerRow = cPerRow;
	color       = Vector4D ( 1.0f, 1.0f, 0.0f, 0.75f );
}

Font :: ~Font ()
{
	fontImage -> release ();
}

void	Font :: print ( View * view, int x, int y, const char * str, ... )
{
	char	buf [1024];									// hope this wil enough
	va_list	args;

	va_start ( args, str );
	vsprintf_s ( buf, str, args );

	view -> startOrtho ();
														// position the text at (0, 0 - bottomLeft)
	glTranslatef ( (float)x, (float)(view -> getHeight () - y), 0 );

														// save current state and set state for drawing text
	int	blendSrc, blendDst;
	int	texEnv;

	glGetIntegerv ( GL_BLEND_SRC, &blendSrc );
	glGetIntegerv ( GL_BLEND_DST, &blendDst );
	glGetTexEnviv ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texEnv );

	glPushAttrib ( GL_ENABLE_BIT );
	glDisable    ( GL_DEPTH_TEST );
	glDisable    ( GL_LIGHTING );
	glEnable     ( GL_BLEND );

														// setup state for drawing
	if ( texEnv != GL_MODULATE )
		glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glBlendFunc    ( GL_SRC_ALPHA, GL_ONE );
	glBindTexture  ( GL_TEXTURE_2D, fontImage -> getId () );
	glColor4fv     ( color );							// setup color for drawing

														// now goes drawing of the string
	for ( int i = 0; buf [i] != '\0'; i++ )
	{
		int		c  = buf [i] - ' ';
		float	cw = (float) charWidth  / (float) fontImage -> getWidth ();
		float	ch = (float) charHeight / (float) fontImage -> getHeight ();
		float	cx = (c % charsPerRow) * cw;			// get character image index into the fontImage
		float	cy = (c / charsPerRow) * ch;

		glBegin      ( GL_QUADS );
		glTexCoord2f ( cx, cy + ch );					// bottom left
		glVertex2i   ( 0, 0 );
		glTexCoord2f ( cx + cw, cy + ch );				// bottom right
		glVertex2i   ( charWidth - 1, 0 );
		glTexCoord2f ( cx + cw, cy );					// top right
		glVertex2i   ( charWidth - 1, charHeight - 1 );
		glTexCoord2f ( cx, cy );						// top left
		glVertex2i   ( 0, charHeight - 1 );
		glEnd        ();
		glTranslatef ( (float)spacing, 0.0f, 0.0f );
	}
														// restore state
	if ( texEnv != GL_MODULATE )
		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texEnv );

	glBlendFunc ( blendSrc, blendDst );					// now restore state

	glPopAttrib  ();

	view -> endOrtho ();
}

