//
// Class for generic array
//
// Author: Alex V. Boreskoff
//
// Last change: 2/12/2002
//

#include	<malloc.h>
#include	<string.h>
#include	<stdlib.h>

#include    "Array.h"
#include	"ObjStr.h"

MetaClass	Array :: classInstance ( "Array", &Object :: classInstance );

//////////////////////// Array methods ///////////////////////////////

Array :: Array ( const char * theName, int theDelta ) : Object ( theName )
{
	maxItems   = theDelta;
    numItems   = 0;
    delta      = theDelta;
    items      = (Object **) malloc ( maxItems * sizeof (Object *)  );
	metaClass  = &classInstance;
}

Array :: Array ( const Array& array ) : Object ( array.getName () )
{
	maxItems   = array.maxItems;
    numItems   = array.getCount    ();
    delta      = array.getDelta    ();
    items      = (Object **) malloc ( maxItems * sizeof (Object *) );
	metaClass  = &classInstance;

    if ( items == NULL )
		return;

                    // now copy the items from array                 
    for ( int i = 0; i < numItems; i++ )
		items [i] = array.at ( i ) -> retain ();
}

Array :: ~Array ()
{
	removeAll ();
    free      ( items );
}

bool	Array :: isOk () const
{
    return items != NULL;
}

int	Array :: compare ( const Object * obj ) const
{
	int	res = Object :: compare ( obj );

	if ( res != 0 )						// has different classname
		return res;

	Array * theArray = (Array *) obj;

	for ( int i = 0; i < numItems; i++ )
	{
		if ( i >= theArray -> getCount () )
			return 1;

		res = at ( i ) -> compare ( theArray -> at ( i ) );

		if ( res != 0 )
			return res;
	}

	return 0;
}

long	Array :: hash () const
{
	long	value = 0;

	for ( int i = 0; i < numItems; i++ )
		value += at ( i ) -> hash ();

	return value;
}

Object * Array :: getObjectWithName ( const String& theName ) const
{
	for ( Iterator it = getIterator (); !it.end (); ++it )
		if ( theName == it.value () -> getName () )
			return it.value ();

	return NULL;
}

bool	Array :: operator == ( const Array& array ) const
{
    if ( getCount () != array.getCount () )
        return 0;

    for ( int i = 0; i < getCount (); i++ )
        if ( at ( i ) -> isEqual ( array.at ( i ) ) )
            return false;

    return true;
}

void    Array :: removeAll ()
{
    for ( int i = 0; i < getCount (); i++ )
		at ( i ) -> release ();

    numItems = 0;
}

bool    Array :: removeAtIndex ( int pos )
{
    if ( pos < 0 || pos >= getCount () )
        return 0;

    at ( pos ) -> release ();

    int newCount = getCount () - 1;

    if ( pos < newCount )
        memmove ( &items [pos], &items [pos + 1], (newCount - pos) * sizeof (Object *) );

    numItems--;

    return 1;
}

bool	Array :: removeObjectWithName ( const String& theName )
{
	for ( int i = 0; i < numItems; i++ )
		if ( theName == at ( i ) -> getName () )
			return removeAtIndex ( i );

	return 0;
}

int		Array :: insertNoRetain ( Object * item )
{
	if ( numItems + 1 > maxItems )
		if ( !realloc ( maxItems + delta ) )
			return 0;

	items [numItems++] = item;

	return 1;
}

int		Array :: insert ( Object * item )			// append item, retaining it
{
	if ( numItems + 1 > maxItems )
		if ( !realloc ( maxItems + delta ) )
			return 0;

	items [numItems++] = item -> retain ();

	return 1;
}

int     Array :: atPut ( int pos, Object * item )
{
    if ( pos < 0 )
        pos = 0;
    else
    if ( pos > numItems )
        pos = numItems;

    if ( pos >= maxItems )
        if ( !realloc ( getCount () + delta ) )
            return 0;

    if ( pos < getCount () )						// we're removing an item
	{
		at ( pos ) -> release ();

		numItems--;
	}

    items [pos] = item -> retain ();

    numItems++;

    return 1;
}

int     Array :: atInsert ( int pos, Object * item )
{
    if ( pos < 0 )
        pos = 0;
    else
    if ( pos > numItems )
        pos = numItems;

    if ( getCount () + 1 >= maxItems )
        if ( !realloc ( getCount () + 1 ) )
            return 0;

    if ( pos < getCount () )
        memmove   ( &items [pos + 1], &items [pos], (getCount () - pos) * sizeof (Object *) );

    items [pos] = item -> retain ();

    numItems++;

    return 1;
}

void	Array :: forEach ( bool (*func)( Object * ) ) const
{
	for ( Iterator it = getIterator (); !it.end (); ++it )
		if ( !(*func)( it.value () ) )
			break;
}

int     Array :: realloc ( int newSize )
{
    if ( newSize > maxItems )
    {
        newSize  = ((newSize + delta - 1) / delta) * delta;
        items    = (Object **)::realloc ( items, newSize * sizeof (Object *) );
        maxItems = newSize;
    }

    return  items != NULL;
}

int	Array :: indexOfObject ( Object * object ) const
{
	for ( int i = 0; i < numItems; i++ )
		if ( at ( i ) -> isEqual ( object ) )
			return i;

	return -1;
}

int	Array :: indexOfObjectIdenticalTo ( Object * object ) const
{
	for ( int i = 0;i < numItems; i++ )
		if ( at ( i ) == object )
			return i;

	return -1;
}
	
static	ObjectComparator	theComparator;
static	void              * theArg;

static	int __cdecl compFunc ( const void * elem1, const void * elem2 )
{
	return (*theComparator)( (Object *)elem1, (Object *)elem2, theArg );
}

void	Array :: sort ( ObjectComparator func, void * arg )
{
			// NOT a thread-safe approach
	theComparator = func;
	theArg        = arg;

	qsort ( items, numItems, sizeof (Object *), compFunc );
}

