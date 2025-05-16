//
// Basic Dictionary class for objects
// 
// Author: Alex V. Boreskoff
//
// Last modified: 21/11/2002
//

#ifndef	__DICTIONARY__
#define	__DICITONARY__

#include	"Object.h"
#include	"Pool.h"

class	Dictionary : public Object
{
	struct	HashItem
	{
		HashItem  * next;					// pointer to next item in chain
		Object    * key;					// key object
		Object    * data;					// object being stored
		long		hashValue;				// hash value if the key
	};
private:
	HashItem      ** hashTable;				// hash table
	int			     hashSize;				// table contains hashSize elements
	int				 numItems;				// # of items currently in the dictionary

	static	MemoryPool	pool;				// pool of HashItems

public:
	class	Iterator						// dictionary iterator
	{
	private:
		const Dictionary * dict;			// dicitonary we're iterating on
		int		           hashIndex;		// index into hash table
		HashItem         * item;			// current hash item
	public:
		Iterator ( const Dictionary * );

		Object * key () const
		{
			return item -> key;
		}

		Object * value () const
		{
			return item -> data;
		}

		bool	end () const
		{
			return hashIndex >= dict -> hashSize;
		}

		void	operator ++ ();
	};

	friend	class	Iterator;

	Dictionary ();
	Dictionary ( const Dictionary& theDict );
	Dictionary ( const char * theName, int theHashSize = 1000 );
	~Dictionary ();

    virtual bool isOk () const
    {
        return hashTable != NULL && hashSize > 0;
    }

	virtual	int		compare ( const Object * obj ) const;

	virtual	long	hash () const
	{
		return numItems;
	}

    bool	operator == ( const Dictionary& dict ) const
	{
		return compare ( &dict ) == 0;
	}

    bool   operator != ( const Dictionary& dict ) const
    {
        return ! operator == ( dict );
    }

	Object * itemForKey ( const Object& key ) const			// return item for given key on NULL
	{
		return itemForKey ( &key );
	}

	void	 removeObject ( const Object& key )
	{
		removeObject ( &key );
	}

	Object * itemForKey   ( const Object * key ) const;		// return item for given key on NULL
	int		 insert       ( Object * key, Object * item );	// insert item at appropriate position
	bool	 removeObject ( const Object * key );			// delete item for given key
	void	 removeAll    ();								// delete all items	

	void	forEach ( bool (*func)( Object *, Object * ) ) const;

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

	Array * getKeys  () const;					// return array of all keys in the dictionary
	Array * getItems () const;					// return array of all items in the dictionary

	static	MetaClass	classInstance;
};

#endif
