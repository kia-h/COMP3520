/**
 * COMP3520 - Operating Systems - Assignment 2
 * 
 * author: Internet's own boy
 */

/* mab - memory management functions for HOST dispatcher */

#include <stdlib.h>
#include <stdio.h>

 #include "../header/memoryblock.h"

/* Function to check malloc outcome... this was taken from assignment 1 exercises, provided by tutor */
/* this function checks for dynamic memory allocation status
 * it retrun nothing on success
 * it prints out error message and exit the program on failure
 */ 
void checkMalloc(void * ptr) 
{
  if (ptr == NULL) 
  {
    fprintf(stderr, "FATAL: Dynamic memory allocation failure\n");
    exit(1);
  }
}

/* 
 * Function: create_mab
 * --------------------
 *	creates MAB (memory allocation block) 
 * 
 *	size: size of memory block in Mbytes 
 *
 *	returns: a pointer to the memory blockhead   
 */
MabPtr create_mab(int size) 
{
	//allocate memory and check
	MabPtr tempBlock = (MabPtr)(malloc(sizeof(Mab)));
	checkMalloc((void*) tempBlock);

	//initialize the details with default values
	tempBlock->processId = 0;
	tempBlock->offset = 0;
	tempBlock->size = size;
	tempBlock->allocated = 0;
	tempBlock->previous = NULL;
	tempBlock->next = NULL;
	return tempBlock;
}

/* 
 * Function: check_mab
 * --------------------
 *	checks if memory with given size is available 
 * 
 *	memoryBlock: memory block to check
 *	size: size of memory block in Mbytes 
 *
 *	returns: a pointer to the memory block   
 */
MabPtr check_mab(MabPtr memoryBlock, int size) 
{
	while (memoryBlock) 
	{
		if (memoryBlock->allocated == FALSE && memoryBlock->size >= size) 
		{
			return memoryBlock;
		}
		memoryBlock = memoryBlock->next;
	}
	return NULL;
}

/* 
 * Function: allocate_mab
 * ----------------------
 *	allocates memory block with the size given
 * 
 *	memoryBlock: memory block to check
 *	size: size of memory block in Mbytes 
 *
 *	returns: a pointer to the memory block   
 */
/* */
MabPtr allocate_mab(MabPtr memoryBlock, int size)
{
	//do so if size is positive
	if (size > 0) 
	{ 
		//check for memory availability
		MabPtr temp = check_mab(memoryBlock,size);

		//if we're good, allocate the requested memory
		if (temp) 
		{
			//split the block if necessary
			temp = split_mab(temp,size);
			temp->allocated = TRUE;
			return temp;
		}
	}
	//we have problem and we just return NULL
	return NULL;
}

/* Free all used memory blocks */
MabPtr freeAll_mab(MabPtr memoryBlock) 
{
	memoryBlock = getHead_mab(memoryBlock);
	MabPtr next;
	while (memoryBlock) 
	{
		next = memoryBlock->next;
		free(memoryBlock);
		memoryBlock = next;
	}
	next = NULL;
	memoryBlock = NULL;
	return NULL;
}

/* 
 * Function: free_mab
 * ------------------
 *	Free a memory block
 * 
 *	memoryBlock: memory block to free
 *
 *	returns: pointer to head of new block  
 */
MabPtr free_mab(MabPtr memoryBlock)
{
	//check if memory block is not null
	if (memoryBlock != NULL)
	{
		//set the staus value
		memoryBlock->allocated = FALSE;
		MabPtr previous = memoryBlock->previous;
		// merge with next block
		memoryBlock = merge_mab(memoryBlock);

		// if merging with next was unsuccessful, try to merge with prev 
		previous = merge_mab(previous); 
	}
	return NULL;
}

/* 
 * Function: merge_mab
 * -------------------
 *	Merge a memory block with next one in the queue if possible
 * 
 *	memoryBlock: memory block to merge
 *
 *	returns: pointer to head of new block  
 */
MabPtr merge_mab(MabPtr memoryBlock)
{
	//check for possiblility of merge
	//if current block and next one are not allocated then do the merge
	if (memoryBlock && memoryBlock->next && memoryBlock->allocated == FALSE && memoryBlock->next->allocated == FALSE) 
	{ 
		MabPtr temp = memoryBlock->next;
		memoryBlock->size = memoryBlock->size + temp->size;
		memoryBlock->next = temp->next;
		if (temp->next) 
		{
			temp->next->previous = memoryBlock;
		}
		free(temp);
		return memoryBlock;
	}
	return NULL;
}

/* 
 * Function: split_mab
 * -------------------
 *	splits the memory block into 2 blocks with following sizes
 *  size and (m->size - size)
 * 
 *	memoryBlock: memory block to split
 *	size       : size of memory block
 *
 *	returns: pointer to head of new block  
 */
MabPtr split_mab(MabPtr memoryBlock, int size)
{
	//check if split is possible
	if (memoryBlock->size == size) 
	{
		return memoryBlock;
	}

	//we can split
	// Create new memory blocks 
	MabPtr first = create_mab(size);
	MabPtr second = create_mab(memoryBlock->size - size);
	
	//if given block has previous node, set the pointer to new one..first
	first->previous = memoryBlock->previous;
	if (first->previous) 
	{
		first->previous->next = first;
	}

	//same goes for next pointers
	first->next = second;
	second->previous = first;
	second->next = memoryBlock->next;
	if (second->next) 
	{
		second->next->previous = second;
	}
	//Set offsets addresses accordingly
	first->offset = memoryBlock->offset;
	second->offset = first->offset + size;

	//free the original memory block and return a newly created block.. first one...
	free(memoryBlock);
	return first;
}

/* 
 * Function: getHead_mab
 * ---------------------
 *	splits the memory block into 2 blocks with following sizes
 *  size and (m->size - size)
 * 
 *	memoryBlock: memory block 
 *
 *	returns: pointer to head of new block  
 */
/* Get memory blocks queue head */
MabPtr getHead_mab(MabPtr memoryBlock) 
{
	MabPtr temp = memoryBlock;

	//iterate over the queue
	while (temp) 
	{
		//single node
		if (temp->previous == NULL) 
		{
			return temp;
		}

		//otherwise we have more, move on
		temp = temp->previous;
	}
	
	return temp; 
}