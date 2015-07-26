/**
 * COMP3520 - Operating Systems - Assignment 2
 * 
 * author: Internet's own boy
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../header/dispatcher.h"
#include "../header/memoryblock.h"
#include "../header/resource.h"

// Global Variables 

int timer;

//to store all jobs from input file 
PcbPtr jobDispatch_queue = NULL; 

//to store real time jobs
PcbPtr realTimeJobs_queue = NULL; 

//to store user jobs
PcbPtr userJobs_queue = NULL; 

//we have 3 different priorities queues. 1 being highest and 3 being lowest
PcbPtr feedback_queue_1 = NULL;
PcbPtr feedback_queue_2 = NULL;
PcbPtr feedback_queue_3 = NULL;

//other variables...
PcbPtr currentJob = NULL; 
RsrcPtr resources = NULL; 
MabPtr memoryHead = NULL; 

/* 
 * Function: handleCurrentJob
 * --------------------------
 *	handles the current job based on its remaining time and priority 
 * 
 *	Input: N/A
 *
 *	returns: a pointer to the current process control block   
 */
PcbPtr handleCurrentJob() 
{
	// check for any running process 
	if (currentJob) 
	{ 
		currentJob->remainingCpuTime--; 

		//check current process remaining time 
		if ((currentJob->remainingCpuTime <= 0) || 
			  (currentJob->initialCpuTime - currentJob->remainingCpuTime >= 20)
			 ) 
		{ 
			//time is up for this process, so terminate it
			terminate_pcb(currentJob);

			//if it's not a real time job then claim its resources
			if (currentJob->priority > 0) 
			{ 
				resources = claim_resource(resources,currentJob->requestedResources);
			}

			// and free up its used memory
			if (currentJob->priority > 0) 
			{
				free_mab(currentJob->memoryBlock);
			}
			free_pcb(currentJob);
			currentJob = NULL;
		}

		//check other jobs queues
		else if (realTimeJobs_queue || feedback_queue_1 || feedback_queue_2 || feedback_queue_3)
		{ 
			//check if it's a real time job
			if (currentJob->priority == 0) 
			{ 
				return currentJob;
			}

			// suspend the job 
			currentJob = suspend_pcb(currentJob);
			
			//check its prioirty and decrease it if possible
			if (currentJob-> priority < 3 && currentJob->priority != 0)
			{ 
				currentJob->priority = currentJob->priority +1 ;
			}

			//check the update priority, and add it to one of priority queues if possible
			if (currentJob->priority > LOW_PRIORITY || currentJob->priority < HIGH_PRIORITY) 
			{
				fprintf(stderr, "Failure! invalid process priority value. - id: %d, pid: %d \n", currentJob->id,currentJob->pid);
				exit(-1);
			}
			if (currentJob->priority==1) 
			{
				feedback_queue_1 = addToHead_pcb(feedback_queue_1,currentJob);
			}
			else if (currentJob->priority==2)
			{
				feedback_queue_2 = addToHead_pcb(feedback_queue_2,currentJob);	
			}
			else if (currentJob->priority==3)
			{
				feedback_queue_3 = addToHead_pcb(feedback_queue_3,currentJob);				
			}

			//we should not reach here
			currentJob = NULL;

		}//end of inner else if

	}//end of outer if
	return currentJob;
}

/* 
 * Function: startJob
 * ------------------
 *	Starts next available job in queues. i.e. real itme or priority queues  
 * 
 *	Input: N/A
 *
 *	returns: a pointer to the current process control block   
 */
PcbPtr startJob() 
{
	//check the current job and queues
	if (currentJob == NULL && (realTimeJobs_queue || 
		  feedback_queue_1   || feedback_queue_2    || feedback_queue_3)
		 ) 
	{ 
		//check if there is any real time job
		if (realTimeJobs_queue) 
		{
			currentJob = dequeue_pcb(&realTimeJobs_queue);
			currentJob->memoryBlock = memoryHead;
		}

		//otherwise check other priority queues
		else if (feedback_queue_1) 
		{
			currentJob = dequeue_pcb(&feedback_queue_1);
		}
		else if (feedback_queue_2) 
		{
			currentJob = dequeue_pcb(&feedback_queue_2);
		}
		else if (feedback_queue_3) 
		{
			currentJob = dequeue_pcb(&feedback_queue_3);
		}
		else 
		{
			fprintf(stderr, "Error!. There are no jobs in priorities queues.\n");
		}

		//we're good here, start the job and return it
		start_pcb(currentJob);
		return currentJob;
	}

	//we shouldn't reach here
	return NULL;
}

/* 
 * Function: roundRobinJobsEnqueue
 * -------------------------------
 *	Puts jobs from user jobs queue to one of priorities queues 
 * 
 *	Input: N/A
 *
 *	returns: N/A   
 */
void roundRobinJobsEnqueue() 
{
	PcbPtr process;

	// iterate over user jobs queue 
	while (userJobs_queue) 
	{ 
		// check to see if we have available resource(s) or memory 
		if (check_resource(resources,userJobs_queue->requestedResources) == 1 && 
				check_mab(memoryHead,userJobs_queue->mbytes)
			 )
		{ 
			//get the job from queue
			process = dequeue_pcb(&userJobs_queue);

			//allocate memory 
			process->memoryBlock = allocate_mab(memoryHead,process->mbytes); 
			
			//check for memory allocation outcome
			if (process->memoryBlock) 
			{ 
				process->memoryBlock->processId = process->id;
			}

			//allocate resources 
			resources = allocate_resource(resources,process->requestedResources);
																											 

			//check the updated priority, and add it to one of priority queues if possible
			if (process->priority > LOW_PRIORITY || process->priority < HIGH_PRIORITY) 
			{
				fprintf(stderr, "Error! invalid process priority value. - id: %d, pid: %d \n", currentJob->id,currentJob->pid);
				exit(-1);
			}
			if (process->priority==1) 
			{
				feedback_queue_1 = addToHead_pcb(feedback_queue_1,process);
			}
			else if (process->priority==2)
			{
				feedback_queue_2 = addToHead_pcb(feedback_queue_2,process);	
			}
			else if (process->priority==3)
			{
				feedback_queue_3 = addToHead_pcb(feedback_queue_3,process);				
			}

		}//end of outer if

		//there are no resources or memory available
		else 
		{ 
			return;
		}
	}//end of while loop
}//end of function

/* 
 * Function: userOrRealTimeJobsEnqueue
 * -----------------------------------
 *	Puts jobs from job dispatch queue to either user jobs queue or real time jobs queue
 *	
 *	Input: N/A
 *
 *	returns: N/A   
 */
void userOrRealTimeJobsEnqueue()
{
	PcbPtr process;

	//iterate over jobs dispatch queue
	while(jobDispatch_queue && jobDispatch_queue->arrivalTime <= timer) 
	{ 
		//get the job from dispacth queue
		process = dequeue_pcb(&jobDispatch_queue);

		//check its priority. 0 goes into real time queue, others go to user jobs queue
		if (process->priority == 0) 
		{ 
			realTimeJobs_queue = addToHead_pcb(realTimeJobs_queue,process);
		}
		else 
		{
			userJobs_queue = addToHead_pcb(userJobs_queue,process);
		}
	}//end of while loop
}

/* 
 * Function: dispatcher
 * --------------------
 *	schedules and manages the jobs in the queues 
 *	
 *	queue: jobs queues
 *
 *	returns: N/A   
 */
void dispatcher(PcbPtr queue) 
{
	timer = 0;

	//our initial dispatch jobs queue
	jobDispatch_queue = queue;

	//create resources we need
	resources = create_resource(MAX_PRINTERS,MAX_SCANNERS,MAX_MODEMS,MAX_CDS);

	//allocate memory for real time jobs
	memoryHead= create_mab(REAL_TIME_MEMORY_SIZE + USER_MEMORY_SIZE);
	memoryHead = allocate_mab(memoryHead,REAL_TIME_MEMORY_SIZE); 
	
	//00 for real time jobs, we could have another value
	memoryHead->processId = 00; 

	//iterate over all the jobs queues
	while (jobDispatch_queue || userJobs_queue  || realTimeJobs_queue || 
		     currentJob 			 || 
		     feedback_queue_1	 || feedback_queue_2|| feedback_queue_3
		    ) 
	{ 
		// transfer jobs from dispatch jobs queue to either user jobs queue or real time jobs queue
		userOrRealTimeJobsEnqueue(); 

		//transfer jobs to one of priorities queues
		roundRobinJobsEnqueue(); 

		//handle the current job. i.e. update its time or suspend it
		currentJob = handleCurrentJob(); 

		//start the next job in one of priorities queue 
		startJob(); 
		sleep(1);
		timer++;
	}

	//free up any used memory or resources
	freeAll_mab(memoryHead); 
	free_resource(resources); 
}