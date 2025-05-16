//
// Class for allocating a lot of equally-sized chunks of memory.
// Automatically allocates new blocks of memory, when they are needed.
//
// Author: Alex V. Boreskoff
//
// last modified: 6/12/2001
//

#ifndef	__POOL__
#define	__POOL__

class	MemoryBlock;

class	MemoryPool					// pool allocator for spans
{
private:
	int	          blockSize;		// # of items per block
	int			  itemSize;			// size of a single item
	MemoryBlock * block;			// pointer to 1st span block
	int			  itemsAllocated;
public:
	MemoryPool ( int theItemSize, int delta );
	~MemoryPool ();

	void	freeAll   ();				// free all spans
	void  * allocItem ();				// allocate unused item
	void	freeItem  ( void * item );	// deallocate span (return to free span list)

	int	getAllocatedItemsCount () const
	{
		return itemsAllocated;
	}
};

#endif
