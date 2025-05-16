//
// Class for allocating a lot of equally-sized chunks of memory.
// Automatically allocates new blocks of memory, when they are needed.
//
// Author: Alex V. Boreskoff
//
// last modified: 6/12/2001
//

#include	<malloc.h>
#include	<stdio.h>				// for NULL
#include	"Pool.h"

/////////////////////// here goes MemoryBlock class definitions and methods /////////////////////

class	MemoryBlock					// block of contineous memory with allocation.deallocation reoutines
{
private:
	void        * pool;				// pointer to the start of block
	int			  itemSize;			// size of single item in bytes
	int			  blockSize;		// # of bytes in the block
	MemoryBlock * nextBlock;		// pointer to next block
	void        * firstAvailable;	// pointer to 1st byte after all allocated
	void        * lastAvailable;	// pointer to last item in the block
	void        * free;				// pointer to free structs list (below firstAvailable)
public:
	MemoryBlock ( int theItemSize, int maxItems )
	{
		itemSize       = theItemSize;
		blockSize      = itemSize * maxItems;
		pool           = malloc ( blockSize );
		firstAvailable = pool;
		lastAvailable  = blockSize - itemSize + (char *) pool;
		free           = NULL;
		nextBlock      = NULL;
	}

	~MemoryBlock ()
	{
		:: free ( pool );
	}

	MemoryBlock * getNext () const
	{
		return nextBlock;
	}

	void	setNext ( MemoryBlock * next )
	{
		nextBlock = next;
	}

	void	freeAll ()				// free all items in the block
	{
		firstAvailable = pool;
		free           = NULL;
	}
									
	bool	contains ( void * ptr ) const
	{
		return ptr >= pool && ptr <= lastAvailable;
	}

	void * allocItem ()				// allocate unused block
	{
		if ( free == NULL )			// if no free items
			if ( firstAvailable <= lastAvailable )
			{
				void	* ptr = firstAvailable;

				firstAvailable = itemSize + (char *) firstAvailable;

				return ptr;
			}
			else
				return NULL;		// failed to allocate item in this block

									// otherwise traverse free list
		void * res = free;			// save 1st free item

		free = * (void **) free;	// advance free to next item in the list

		return res;
	}
									// deallocate span (add to free span list)
	void	freeItem ( void * item )
	{
		* (void **) item = free;
		free             = item;
	}
};

////////////////////////// MemoryPool methods ////////////////////////////////

MemoryPool :: MemoryPool ( int theItemSize, int delta )
{
	if ( theItemSize < sizeof ( void * ) )
		theItemSize = sizeof ( void * );

	itemSize       = theItemSize;
	blockSize      = delta;
	block          = new MemoryBlock ( itemSize, delta );
	itemsAllocated = 0;
}

MemoryPool :: ~MemoryPool ()
{
	MemoryBlock * cur = block;
	MemoryBlock * next;

	for ( ; cur != NULL; cur = next )
	{
		next = cur -> getNext ();

		delete cur;
	}
}

void	MemoryPool :: freeAll ()				// free all spans
{
	MemoryBlock * cur = block;
	MemoryBlock * next;

	for ( ; cur != NULL; cur = next )
	{
		next = cur -> getNext ();

		cur -> freeAll ();
	}

	itemsAllocated = 0;
}

void * MemoryPool :: allocItem ()				// allocate unused item
{					
	void        * item;
	MemoryBlock * cur = block;

	do							// try all blocks for allocating the span
	{
		item = cur -> allocItem ();
		cur  = cur -> getNext   ();
	} while ( item == NULL && cur != NULL );

	if ( item == NULL )			// append new block
	{
		cur   = block;
		block = new MemoryBlock ( itemSize, blockSize );

		block -> setNext ( cur );

		item = block -> allocItem ();
	}

	itemsAllocated++;

	return item;
}
											// deallocate span (return to free span list)
void	MemoryPool :: freeItem ( void * ptr )		
{
	itemsAllocated--;

	for ( MemoryBlock * cur = block; cur != NULL; cur = cur -> getNext () )
		if ( cur -> contains ( ptr ) )
		{
			cur -> freeItem ( ptr );

			return;
		}
}

