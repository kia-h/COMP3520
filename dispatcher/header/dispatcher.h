/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

/*******************************************************************

   Assignment 2 - HOST dispatcher

  dispatcher.h - dispatcher functions for HOST dispatcher

	void userOrRealTimeJobsEnqueue(); - put jobs from dispatch queue into user jobs or real time jobs queues
	void roundRobinEnqueue();					- put jobs from user queue into priorities queues
	PcbPtr handleCurrentJob();				- control the job behaviour
	PcbPtr startJob();								- start next available job in queue
	void dispatcher(PcbPtr queue);		- manage the jobs

  see dispatcher.c for fuller description of function arguments and returns
	p.s. some text taken from course provided sample code like this box style
 ********************************************************************/

#include  "processblock.h"

/******* dispatcher function prototypes *******/

void 	 userOrRealTimeJobsEnqueue();
void 	 roundRobinEnqueue();
PcbPtr handleCurrentJob();
PcbPtr startJob();
void 	 dispatcher(PcbPtr );

#endif