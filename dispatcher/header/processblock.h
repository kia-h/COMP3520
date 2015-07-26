/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#ifndef PROCESSBLOCK_H
#define PROCESSBLOCK_H

/*******************************************************************

   Assignment 2 - HOST dispatcher

  processblock.h - process control block functions for HOST dispatcher

	PcbPtr start_pcb(PcbPtr );							- start a job	
	PcbPtr suspend_pcb(PcbPtr );						- suspend a job 
	PcbPtr terminate_pcb(PcbPtr );					- terminate a job
	void print_pcb(PcbPtr );								- print process details
	
	PcbPtr create_pcb(int , int , int , int , int , int , int , int ); - create a job
	PcbPtr addToTail_pcb(PcbPtr , PcbPtr ); - add a process to end of queue
	PcbPtr addToHead_pcb(PcbPtr , PcbPtr ); - add a job to head of queue
	PcbPtr getTail_pcb(PcbPtr );						- get tail in jobs queue
	PcbPtr dequeue_pcb(PcbPtr *);						- remove a job from queue
	
	void free_pcb(PcbPtr );									- free memory for a single job
	PcbPtr freeAll_pcb(PcbPtr );						- free memory for whole queue

  see processblock.c for fuller description of function arguments and returns
	p.s. some text taken from course provided sample code like this box style.
	     this header structure has been provided by course teaching staff,
	     however i changed it. so there might be some similarities of course
 ********************************************************************/

#include <unistd.h>

#include "../header/resource.h"
#include "../header/memoryblock.h"

/******* resource management definitions*******/

#define MAX_ARGS	3 
 
#define MAX_INPUT_SIZE 1024		
#define DEFAULT_PROCESS "./sigtrap"	

#define HIGH_PRIORITY 1
#define LOW_PRIORITY 	3

/* List of Status for PCB */
#define ERROR 			 -1
#define UNINITIALIZED 0
#define INITIALIZED 	1
#define READY 				2
#define RUNNING 			3
#define SUSPENDED 		4
#define TERMINATED 		5

struct pcb 
{	
	//id for our own purposes
	int id; 

	pid_t pid; 	
	
	char * args[MAX_ARGS]; 	

	int arrivalTime;
	int priority;
	int initialCpuTime;
	int remainingCpuTime; 

	int mbytes;
	MabPtr memoryBlock; 
	int status;

	RsrcPtr requestedResources; 

	struct pcb * previous;
	struct pcb * next;
};

typedef struct pcb Pcb;
typedef Pcb * PcbPtr;

/******* process management function prototypes *******/

PcbPtr start_pcb 		(PcbPtr );
PcbPtr suspend_pcb 	(PcbPtr );
PcbPtr terminate_pcb(PcbPtr );
void 	 print_pcb 		(PcbPtr );

PcbPtr create_pcb		 (int , int , int , int , RsrcPtr );
PcbPtr addToTail_pcb (PcbPtr , PcbPtr );
PcbPtr addToHead_pcb (PcbPtr , PcbPtr );
PcbPtr getTail_pcb 	 (PcbPtr );
PcbPtr dequeue_pcb 	 (PcbPtr *);

void 	 free_pcb 	(PcbPtr );
PcbPtr freeAll_pcb(PcbPtr );

#endif