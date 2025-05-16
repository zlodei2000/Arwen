//
// Basic Dictionary class for objects
// 
// Author: Alex V. Boreskoff
//
// Last modified: 21/11/2002
//

#include	<memory.h>
#include	<stdlib.h>
#include	"Array.h"
#include	"Dictionary.h"

#define	DEF_SET_SIZE	1024

MemoryPool	Dictionary :: pool          ( sizeof ( HashItem ), 100 );
MetaClass	Dictionary :: classInstance ( "Dictionary", &Object :: classInstance );

Dictionary :: Dictionary () : Object ( "Default dictionary" )
{
	hashSize  = DEF_SET_SIZE;
	numItems  = 0;
	hashTable = new HashItem * [hashSize];
	metaClass = &classInstance;

	if ( hashTable != NULL )
		memset ( hashTable, '\0', hashSize * sizeof ( HashItem * ) );
}

Dictionary :: Dictionary ( const Dictionary& theDict ) : Object ( theDict.getName () )
{
	hashSize  = theDict.hashSize;
	numItems  = 0;
	hashTable = new HashItem * [hashSize];
	metaClass = &classInstance;

	if ( hashTable == NULL )
		return;

	memset ( hashTable, '\0', hashSize * sizeof ( HashItem * ) );

	for ( Iterator it = theDict.getIterator (); !it.end (); ++it )
		insert ( it.key (), it.value () );
}

Dictionary :: Dictionary ( const char * theName, int theHashSize ) : Object ( "Default set" )
{
	hashSize  = theHashSize;
	numItems  = 0;
	hashTable = new HashItem * [hashSize];
	metaClass = &classInstance;

	if ( hashTable != NULL )
		memset ( hashTable, '\0', hashSize * sizeof ( HashItem * ) );
}

Dictionary :: ~Dictionary ()
{
	removeAll ();

	delete hashTable;
}

int		Dictionary :: compare ( const Object * obj ) const
{
	return Object :: compare ( obj );

	// XXX
}

Object * Dictionary :: itemForKey ( const Object * key ) const
{
	if ( hashTable == NULL )
		return NULL;

	long	hashValue = key -> hash ();
	int		pos       = abs ( hashValue ) % hashSize;			// index to hashTable

	for ( HashItem * ptr = hashTable [pos]; ptr != NULL; ptr = ptr -> next )
	{
		if ( hashValue == ptr -> hashValue )
			if ( key -> isEqual ( ptr -> key ) )	// found key
				return ptr -> data;
	}

	return NULL;
}

int	Dictionary :: insert ( Object * key, Object * item )
{
	if ( hashTable == NULL )
		return 0;

	long	hashValue = key -> hash ();
	int		pos       = abs ( hashValue ) % hashSize;

														// check for item already present
	for ( HashItem * ptr = hashTable [pos]; ptr != NULL; ptr = ptr -> next )
	{
		if ( hashValue == ptr -> hashValue )
			if ( key -> isEqual ( ptr -> key ) )
				if ( item -> isEqual ( ptr -> data ) )
					return numItems;
	}

														// not present, insert
	HashItem* ptr = (HashItem *) pool.allocItem ();

	ptr -> key       = key  -> retain ();				// retain the key
	ptr -> data      = item -> retain ();				// retain the item
	ptr -> hashValue = hashValue;
	ptr -> next      = hashTable [pos];

	hashTable [pos] = ptr;

	return ++numItems;
}

bool	Dictionary :: removeObject ( const Object * key )
{
	if ( hashTable == NULL )
		return false;

	long	   hashValue = key -> hash ();
	int	       pos  = abs ( hashValue ) % hashSize;
	HashItem * ptr  = hashTable [pos];
	HashItem * prev = NULL;

	while ( ptr != NULL )
	{
		if ( hashValue == ptr -> hashValue )
			if ( key -> isEqual ( ptr -> key ) )
			{
				if ( prev == NULL )			// 1st item in chain
					hashTable [pos] = ptr -> next;
				else
					prev -> next = ptr -> next;

				ptr -> data -> release ();
				ptr -> key  -> release ();

				pool.freeItem ( ptr );

				numItems--;

				return true;
			}

		prev = ptr;
		ptr  = ptr -> next;
	}

	return false;
}

void	Dictionary :: removeAll ()
{
	if ( hashTable == NULL )
		return;

	for ( int i = 0; i < hashSize; i++ )
	{
		HashItem * ptr = hashTable [i];

		while ( ptr != NULL )
		{
			HashItem * next = ptr -> next;

			ptr -> data -> release ();
			ptr -> key  -> release ();

			pool.freeItem ( ptr );

			ptr = next;
		}

		hashTable [i] = NULL;
	}

	numItems = 0;
}

void	Dictionary :: forEach ( bool (*func)( Object *, Object * ) ) const
{
	for ( Iterator it = getIterator (); !it.end (); ++it )
		if ( !(*func)( it.key (), it.value () ) )
			break;
}

Array * Dictionary :: getKeys () const
{
	Array * keys = new Array;

	for ( Iterator it ( this ); !it.end (); ++it )
		keys -> insert ( it.key () );

	return keys;
}

Array * Dictionary :: getItems () const
{
	Array * items = new Array;

	for ( Iterator it ( this ); !it.end (); ++it )
		items -> insert ( it.value () );

	return items;
}

///////////////////////////// methods of Dictionary :: Iterator /////////////////////////////////

Dictionary :: Iterator :: Iterator ( const Dictionary * theDict )
{
	dict      = theDict;
	hashIndex = 0;
	
	while ( ( item = dict -> hashTable [hashIndex] ) == NULL )
		if ( ++hashIndex >= dict -> hashSize )
			return;
}

void	Dictionary :: Iterator :: operator ++ ()
{
	if ( item == NULL )
		return;						// must be some error or end of set

	item = item -> next;

	while ( item == NULL )
	{
		if ( ++hashIndex >= dict -> hashSize )
			return;

		item = dict -> hashTable [hashIndex];
	}
}
