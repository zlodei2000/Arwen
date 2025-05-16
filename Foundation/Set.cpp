//
// Set class based on hash table
//
// Author: Alex V. Boreskoff
//
// Last modified: 16/07/2002
//

#define	DEF_SET_SIZE	1024

#include	<memory.h>
#include	"Array.h"
#include	"Set.h"

MemoryPool	Set :: pool ( sizeof ( HashItem ), 100 );
MetaClass	Set :: classInstance ( "Set", &Object :: classInstance );

Set :: Set () : Object ( "Default set" )
{
	hashSize  = DEF_SET_SIZE;
	numItems  = 0;
	hashTable = new HashItem * [hashSize];

	if ( hashTable != NULL )
		memset ( hashTable, '\0', hashSize * sizeof ( HashItem * ) );
}

Set :: Set ( const Set& theSet ) : Object ( theSet.getName () )
{
	hashSize  = theSet.hashSize;
	numItems  = 0;
	hashTable = new HashItem * [hashSize];

	if ( hashTable == NULL )
		return;

	memset ( hashTable, '\0', hashSize * sizeof ( HashItem * ) );

	for ( Iterator it = theSet.getIterator (); !it.end (); ++it )
		insert ( it.value () );
}

Set :: Set ( const char * theName, int theHashSize ) : Object ( theName )
{
	hashSize  = theHashSize;
	numItems  = 0;
	hashTable = new HashItem * [hashSize];

	if ( hashTable != NULL )
		memset ( hashTable, '\0', hashSize * sizeof ( HashItem * ) );
}

Set :: ~Set ()
{
	removeAll ();

	delete hashTable;
}

bool	Set :: has ( Object * item ) const
{
	if ( hashTable == NULL )
		return false;

	long	 hashValue      = item -> hash ();
	register HashItem * ptr = hashTable [hashValue % hashSize];

	while ( ptr != NULL )
	{
		if ( hashValue == ptr -> hashValue )					// compare only if hashes are equal
			if ( item -> isEqual ( ptr -> data ) )				// found item
				return true;

		ptr = ptr -> next;										// advance to next item in list
	}

	return false;
}

int	Set :: insert ( Object * item )
{
	if ( hashTable == NULL )
		return 0;

	long	hashValue = item -> hash ();
	int		pos       = hashValue % hashSize;

	for ( register HashItem * ptr = hashTable [pos]; ptr != NULL; ptr = ptr -> next )
	{
		if ( hashValue == ptr -> hashValue )
			if ( item -> isEqual ( ptr -> data ) )
				return numItems;
	}

	HashItem* ptr = (HashItem *) pool.allocItem ();

	ptr -> data      = item -> retain ();
	ptr -> hashValue = hashValue;
	ptr -> next      = hashTable [pos];

	hashTable [pos] = ptr;

	return ++numItems;
}

bool	Set :: remove ( Object * item )
{
	if ( hashTable == NULL )
		return false;

	long	   hashValue = item -> hash ();
	int	       pos       = hashValue % hashSize;
	HashItem * ptr       = hashTable [pos];
	HashItem * prev      = NULL;

	while ( ptr != NULL )
	{
		if ( hashValue == ptr -> hashValue )
			if ( item -> isEqual ( ptr -> data ) )
			{
				if ( prev == NULL )			// 1st item in chain
					hashTable [pos] = ptr -> next;
				else
					prev -> next = ptr -> next;

				ptr -> data -> release ();

				pool.freeItem ( ptr );

				numItems--;

				return true;
			}

		prev = ptr;
		ptr  = ptr -> next;
	}

	return false;
}

void	Set :: removeAll ()
{
	if ( hashTable == NULL )
		return;

	for ( register int i = 0; i < hashSize; i++ )
	{
		register HashItem * ptr = hashTable [i];

		if ( ptr == NULL )
			continue;

		while ( ptr != NULL )
		{
			HashItem * next = ptr -> next;

			ptr -> data -> release ();

			pool.freeItem ( ptr );

			ptr = next;
		}

		hashTable [i] = NULL;
	}

	numItems = 0;
}

void	Set :: forEach ( bool (*func)( Object * ) ) const
{
	for ( Iterator it = getIterator (); !it.end (); ++it )
		if ( !(*func)( it.value () ) )
			break;
}

int	Set :: compare ( const Object * obj ) const
{
	int	res = Object :: compare ( obj );

	if ( res != 0 )
		return res;

	Set * otherSet = (Set *) obj;

	if ( hashSize < otherSet -> hashSize )
		return -1;

	if ( hashSize > otherSet -> hashSize )
		return 1;

	for ( int i = 0; i < hashSize; i++ )
	{
		
	}

	return 0;
}

Array * Set :: getItems () const
{
	Array * array = new Array;

	for ( Iterator it = getIterator (); !it.end (); ++it )
		array -> insert ( it.value () );

	return array;
}

/////////////////////// methods of Set :: Iterator /////////////////////////

Set :: Iterator :: Iterator ( const Set * theSet )
{
	set       = theSet;
	hashIndex = 0;
	
	while ( ( item = set -> hashTable [hashIndex] ) == NULL )
		if ( ++hashIndex >= set -> hashSize )
			return;
}

void	Set :: Iterator :: operator ++ ()
{
	if ( item == NULL )
		return;						// must be some error or end of set

	item = item -> next;

	while ( item == NULL )
	{
		if ( ++hashIndex >= set -> hashSize )
			return;

		item = set -> hashTable [hashIndex];
	}
}
