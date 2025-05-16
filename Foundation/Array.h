//
// Class for generic array
//
// Author: Alex V. Boreskoff
//
// Last change: 2/12/2002
//

#ifndef __ARRAY__
#define __ARRAY__

#include    "Object.h"

typedef	int (*ObjectComparator)(Object *, Object *, void *);

class	String;

class   Array : public Object
{
protected:
    int       maxItems;				// current capacity of array
    int       numItems;             // current # of items in array
    int       delta;                // by how much items grow array
    Object ** items;                // aray containing items (or pointers to them)
public:
    Array ( const char * theName = "", int theDelta = 50 );
    Array ( const Array& array );
    ~Array ();

    virtual bool	isOk    () const;
	virtual	long	hash    () const;
	virtual	int		compare ( const Object * obj ) const;

	virtual	Object * clone () const
	{
		return new Array ( *this );
	}

	static	Object * createObject ()
	{
		return new Array;
	}

    bool	operator == ( const Array& array ) const;
    bool    operator != ( const Array& array ) const
    {
        return ! operator == ( array );
    }

	Object * getObjectWithName ( const String& theName ) const;

    void    removeAll            ();			// delete all items
    bool	removeAtIndex        ( int pos );	// delete specified item
	bool	removeObjectWithName ( const String& theName );

	int		insertNoRetain ( Object * item );	// insert object withouit retaining it
    int		insert         ( Object * item );	// append item, retaining it

	int     atPut    ( int pos, Object * item );// put item at a given position
	int		atInsert ( int pos, Object * item );

    Object  * at ( int pos ) const				// return item at position
    {
        return pos >= 0 && pos < numItems  ? items [pos] : NULL;
    }

	void	forEach ( bool (*func)( Object * ) ) const;

    int     realloc ( int newSize );			// set new capacity of container

	int	indexOfObject ( Object * ) const;			// object identical vis ItemController :: compare
	int	indexOfObjectIdenticalTo ( Object * ) const;// index of object with identical bytes (using memcpy)
	
	void	sort ( ObjectComparator func, void * arg );

    bool  isEmpty () const
    {
        return numItems < 1;
    }

    int getCount () const
    {
        return numItems;
    }

	int	getNumItems () const
	{
		return numItems;
	}

    int getDelta () const
    {
        return delta;
    }

	class	Iterator
	{
	private:
		const Array * array;
		int			  index;
	public:
		Iterator ( const Array * theArray )
		{
			array = theArray;
			index = 0;
		}

		int	end () const
		{
			return index >= array -> getCount ();
		}

		Object * value () const
		{
			return array -> at ( index );
		}

		void operator ++ ()
		{
			index++;
		}
	};

	Iterator	getIterator () const
	{
		return Iterator ( this );
	}

	static	MetaClass	classInstance;
};

#endif
