//
// Set class for objects based on hash table
//
// Author: Alex V. Boreskoff
//
// Last modified: 16/07/2001
//

#ifndef	__SET__
#define	__SET__

#include	"Object.h"
#include	"Pool.h"

class	Array;

class	Set : public Object
{
	struct	HashItem
	{
		HashItem  * next;					// pointer to next item in chain
		Object    * data;					// object being stored
		long		hashValue;
	};

private:
	HashItem      ** hashTable;
	int			     hashSize;				// table contains hashSize elements
	int				 numItems;

	static	MemoryPool	pool;				// pool of HashItems

public:

	class	Iterator						// set iterator
	{
	private:
		const Set * set;					// set we're iterating on
		int		    hashIndex;				// index into hash table
		HashItem  * item;					// current hash item
	public:
		Iterator ( const Set * );

		Object * value () const
		{
			return item -> data;
		}

		bool	end () const
		{
			return hashIndex >= set -> hashSize;
		}

		void	operator ++ ();
	};

	friend	class	Iterator;

	Set ();
	Set ( const Set& theSet );
	Set ( const char * theName, int theHashSize = 1000 );
	~Set ();

    virtual bool isOk () const
    {
        return hashTable != NULL && hashSize > 0;
    }

	virtual	int		compare ( const Object * obj ) const;

	virtual	long	hash    () const
	{
		return numItems;
	}

    bool	operator == ( const Set& set ) const
	{
		return compare ( &set ) == 0;
	}

    bool	operator != ( const Set& set ) const
    {
        return ! operator == ( set );
    }

	bool	has       ( Object * item ) const;	// return non-negative if set contains item 
	int		insert    ( Object * item );		// insert item at appropriate position
	bool	remove    ( Object * item );		// delete item at specified position
	void	removeAll ();						// delete all items	

	void	forEach ( bool (*func)( Object * ) ) const;

    bool   isEmpty () const
    {
        return numItems < 1;
    }

	int	getNumItems () const
	{
		return numItems;
	}

	Iterator	getIterator () const
	{
		return Iterator ( this );
	}

	Array * getItems () const;

	static	MetaClass	classInstance;
};

#endif
