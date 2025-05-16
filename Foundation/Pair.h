//
// Author: Alex V. Boreskoff
//
// Last modified: 5/11/2001
//

#ifndef	__PAIR__
#define	__PAIR__

#include	"Object.h"

class	Pair : public Object
{
private:
	Object	* first;
	Object	* second;
public:
	Pair ( const char * theName, Object * theFirst, Object * theSecond ) : Object ( theName )
	{
		first     = theFirst  != NULL ? theFirst  -> retain () : NULL;
		second    = theSecond != NULL ? theSecond -> retain () : NULL;
		metaClass = &classInstance;
	}

	~Pair ()
	{
		if ( first != NULL )
			first -> release ();

		if ( second != NULL )
			second -> release ();
	}


    virtual bool isOk () const           // returns non-zero if objects is ok
    {
        return (first != NULL ? first -> isOk () : true) && (second != NULL ? second -> isOk () : true);
    }

    virtual int  init ()               // postconstructor initialization
    {
        return 1;						// XXX
    }

    virtual long hash () const          // return hash for object
    {
        return (first != NULL ? first -> hash () : 0) + (second != NULL ? second -> hash () : 0);
    }

	virtual	int	compare ( const Object * obj ) const;

	virtual	Object * clone () const
	{
		return new Pair ( name, first, second );
	}

	static	MetaClass	classInstance;
};
#endif
