/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "../header/processblock.h"

//Global Variables. this will be used to print out ID column
int localProcessID = 0;

/* Get the queue tail*/
PcbPtr getTail_pcb(PcbPtr head) 
{
	while (head->next != NULL) 
	{
		head = head->next;
	}
	return head;
}

/* dequeue a node from the list and return it */
PcbPtr dequeue_pcb(PcbPtr *head)
{
	//check if queue is empty
	if((*head) == NULL) 
	{
		fprintf(stderr, "Error! Queue is empty.\n");
		return NULL;
	}

	//otherwise, remove the head and set the pointers accordingly
	PcbPtr temp = *head;
	*head = (*head)->next;
	if ((*head) != NULL) 
	{
		(*head)->previous = NULL; 
	}
	temp->next = NULL;
	temp->previous = NULL;
	return temp;
}

/*Add a new node to end of queue and return the new tail*/
PcbPtr addToTail_pcb(PcbPtr tail, PcbPtr newNode) 
{
	newNode->next = NULL;
	tail->next = newNode;
	newNode->previous = tail;
	return newNode;
}

/* Add new node to head of queue and return the new head */
PcbPtr addToHead_pcb(PcbPtr head, PcbPtr newNode)
{
	//if queue is empty
	if (head == NULL) 
	{ 
		newNode->next = NULL;
		newNode->previous = NULL;
		head = newNode;
	}

	//or queue has only one node
	else if (head->next == NULL) 
	{ 
		addToTail_pcb(head,newNode);
	}

	//we have more than one node here
	else 
	{ 
		PcbPtr tail = getTail_pcb(head);
		addToTail_pcb(tail,newNode);
	}
	return head;
}

/* Create new pcb(process control block) and return the address */
PcbPtr create_pcb(int arrivalTime, int priority, int initialCpuTime, int mbytes, RsrcPtr requestedResources)
{
	localProcessID++; 
	PcbPtr temp = (PcbPtr)malloc(sizeof(Pcb));

	// set process detials
	temp->args[0] = (char *)malloc(MAX_INPUT_SIZE);
	strcpy(temp->args[0],DEFAULT_PROCESS);

	temp->args[1] = (char *)malloc(MAX_INPUT_SIZE);
	sprintf(temp->args[1],"%d",initialCpuTime);

	temp->args[2] = NULL;

	temp->id = localProcessID;
	temp->arrivalTime = arrivalTime;
	temp->remainingCpuTime = initialCpuTime;
	temp->initialCpuTime = initialCpuTime;
	temp->priority = priority;
	temp->mbytes = mbytes;
	temp->requestedResources = requestedResources;
	temp->pid = 0;
	temp->status = UNINITIALIZED;
	temp->previous = NULL;
	temp->next = NULL;
	return temp;
}

/* Free memery for a node in queue */
void free_pcb(PcbPtr process) 
{
	if (process != NULL) 
	{
		free(process->args[0]);
		free(process->args[1]);
		free(process);
	}
}

/* Free all memory used for pcb queue */
PcbPtr freeAll_pcb(PcbPtr head) 
{
	if (head == NULL) 
	{
		return head;
	}
	PcbPtr temp;
	while (1) 
	{
		temp = head->next;
		free_pcb(head);
		if (temp == NULL) 
		{
			return temp;
		}
		head = temp;
	}
}

/* Start currecnt job */
PcbPtr start_pcb(PcbPtr process) 
{
	//check the process status
	if (process->status == UNINITIALIZED) 
	{	 
		//set the status and fork a new one
		process->status = RUNNING;
		switch(process->pid = fork()) 
		{
			//child creation failed
			case -1:
				printf("Failed! Couldn't create child process!\n");
				return NULL;
				break;

			// child created successfully 
			case 0:
				process->status = RUNNING;
				
				//print process details and execute it
				print_pcb(process);
				execvp(process->args[0],process->args);
				break;
			default:
				break;
		}
		return process;
	}

	// check if current process is suspended
	else if (process->status == SUSPENDED) 
	{ 
		 // continue whate we were doing
		if (kill(process->pid, SIGCONT)) 
		{
      fprintf(stderr, "Failed! Couldn't restart the process! - id: %d, pid: %d\n", process->id,process->pid);
      return NULL;
    }

    //if we're good set the status and return the process'
		process->status = RUNNING;
		return process;
	}

	//if we get here, it means something bad happened
	else 
	{
		fprintf(stderr, "Failed! process couldn't be started! - id: %d, pid: %d \n", process->id,process->pid);
	}
	return NULL; 
}

/* Terminate a process */
PcbPtr terminate_pcb(PcbPtr process) 
{
	//send SINGINT signal and check the outcome
	if(kill(process->pid,SIGINT) != 0) 
	{ 
		fprintf(stderr, "Failed! process couldn't be terminated! - id: %d, pid: %d \n", process->id,process->pid);
		return NULL;
	}

	//stop the process and set the status accordingly
	int status;
	process->status = TERMINATED;
	waitpid(process->pid,&status,WUNTRACED);
	return process;
}

/* Suspend a process */
PcbPtr suspend_pcb(PcbPtr process) 
{
	//send SIGTSTP and check the outcome
	if(kill(process->pid,SIGTSTP)) 
	{
		fprintf(stderr, "Failed! process couldn't be suspended! - id: %d, pid: %d \n", process->id,process->pid);
	}

	//suspend the process and set the status accordingly
	int status;
	process->status = SUSPENDED;
	waitpid(process->pid,&status,WUNTRACED);
	return process;
}

/* print out process details */
void print_pcb(PcbPtr process)
{
    char *statusText="NOT SET";

    //assign a text value for each status code
    switch(process->status ) 
		{			
			case 0:
				statusText = "UNINITIALIZED";
				break;
			case 1:
				statusText = "INITIALIZED";
				break;
			case 2:
				statusText = "READY";
				break;
			case 3:
				statusText = "RUNNING";
				break;
			case 4:
				statusText = "SUSPENDED";
				break;
			case 5:
				statusText = "TERMINATED";
				break;				
			default:
				statusText = "ERROR";
				break;
		}    
    
    fprintf (stdout, "ID\t pid\t arrive\t prior\t cpu\t offset\t Mbytes\t prn\t scn\t modem\t cd\t status\n");
    fprintf(stdout, "%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %s\t\n",
            process->id, getpid(), process->priority, process->arrivalTime, process->remainingCpuTime, 
            process->memoryBlock->offset, process->mbytes, process->requestedResources->printers, process->requestedResources->scanners, 
            process->requestedResources->modems, process->requestedResources->cds,statusText 
           );
}