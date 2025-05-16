//
// Application class of Arwen project
//
// Author: Alex V. Boreskoff
//

#ifndef	__APPLICATION__
#define	__APPLICATION__

#include	<windows.h>
#include	"Object.h"

class	Model;
class	View;
class	Controller;
class	ResourceManager;

class	Application : public Object
{
protected:
	HINSTANCE		  hInstance;				// windoze's instance
	View            * view;
	Model           * model;
	Controller      * controller;
	ResourceManager * resourceManager;
	bool			  buildLightmaps;			// whether we're asked only to build lightmaps

public:

	Application ( const char * theName, const char * args = "" );
	~Application ();

	ResourceManager * getResourceManager () const
	{
		return resourceManager;
	}

	View * getView () const
	{
		return view;
	}

	Model * getModel () const
	{
		return model;
	}

	Controller * getController () const
	{
		return controller;
	}

	void	okBox ( const char * caption, const char * format, ... ) const;

	virtual	bool	isOk () const;
	virtual	int		run ();

	static	Application * instance;
	static	MetaClass	  classInstance;
};

#endif
