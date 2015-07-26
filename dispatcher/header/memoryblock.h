/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

/*******************************************************************

  Assignment 2 - HOST dispatcher

  memoryblock.h - Memory Allocation definitions and prototypes for HOST dispatcher

	void checkMalloc(void *); 					- check malloc outcome
	MabPtr create_mab(int );  					- create a memory block
	MabPtr check_mab(MabPtr , int );		- check for memory availablability
	MabPtr allocate_mab(MabPtr , int ); - allocate a memory block
	MabPtr freeAll_mab(MabPtr );				- free all allocated memeory blocks
	MabPtr free_mab(MabPtr );						- free a single memory block
	MabPtr merge_mab(MabPtr );					- merge memory block with next one
	MabPtr split_mab(MabPtr , int );		- split a memory block into 2 block
	MabPtr getHead_mab(MabPtr );				- get head of memory blocks queue

  see memoryblock.c for fuller description of function arguments and returns
	p.s. some text taken from course provided sample code like this box style
	     this header structure has been provided by course teaching staff,
	     however i changed it. so there might be some similarities of course
********************************************************************/

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/******* memory management definitions*******/

#define MEMORY_SIZE       1024
#define REAL_TIME_MEMORY_SIZE    64
#define USER_MEMORY_SIZE  (MEMORY_SIZE - REAL_TIME_MEMORY_SIZE)

struct mab 
{
	int processId; 
	int offset;
	int size;
	int allocated;
	
	struct mab * next;	
	struct mab * previous;	
};

typedef struct mab Mab;
typedef Mab * MabPtr;

/******* memory management function prototypes *******/

void checkMalloc(void *); 

MabPtr create_mab (int );
MabPtr getHead_mab(MabPtr );
MabPtr check_mab  (MabPtr , int );

MabPtr allocate_mab(MabPtr , int );
MabPtr free_mab    (MabPtr );
MabPtr freeAll_mab (MabPtr );

MabPtr merge_mab(MabPtr );
MabPtr split_mab(MabPtr , int );

#endif