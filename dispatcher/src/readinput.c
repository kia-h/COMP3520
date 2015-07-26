/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/readinput.h"
#include "../header/memoryblock.h"
#include "../header/resource.h"
#include "../header/processblock.h"

/* 
 * Function: getTotalInputs
 * ------------------------
 *	gets the total number of lines in given dispatch file 
 * 
 *	fileName: name of the input file 
 *
 *	returns: number of lines in the input file   
 */
int getTotalInputs(char* fileName) 
{
	FILE *fp;
	//open the file for read operation
  fp = fopen(fileName, "r"); 

   //check for error
  if(fp == NULL)
  {
    return -1;
  }
	
	int temp;
	int total=0;

	//loop through the file and count the lines
  while ( (temp=fgetc(fp)) != EOF ) 
  {
 		if ( temp == '\n' ) 
 		{
      total++;
    }
  }

	//close the file and return the total number
	fclose(fp);
	return total;
}

/* 
 * Function: checkInputsValues
 * ---------------------------
 *	checks each line to be valid based on assignment spec 
 * 
 *	arrivalTime		 : arrival time of the current process (not negative) 
 *	priority	 		 : currecnt process priority (0-3)
 *	cpuTime		 		 : how much cpu time it needs (not negative)
 *	memoryBlock 	 : how much memory it needs (not negative)
 *	printersNumber : how many printers it needs (if any) (not negative)
 *	scannersNumber : how many scanner it needs (if any) (not negative)
 *	modemsNumber	 : how many modems it needs (if any) (not negative)
 *	cdsNumber			 :how many cds it needs (if any) (not negative)
 *
 *	returns: either 0 (false) or 1 (true)   
 */

int checkInputsValues(int arrivalTime, int priority, int cpuTime, int memoryBlock, int printersNumber, int scannersNumber, int modemsNumber, int cdsNumber) 
{
	
	// check  for negative or out of range values in each line 
	if ( (priority < 0) 			|| (priority > 3) 			|| (arrivalTime < 0) 	||  (cpuTime <= 0) || 
		   (printersNumber < 0) || (scannersNumber < 0) || (modemsNumber < 0) || (cdsNumber < 0)
		 ) 
	{ 
		return 0;
	}

	// check real time job specification
	if (priority == 0) 
	{
		//real time jobs don't need any resources and it memory is less than 64Mbytes
		if ((memoryBlock > 64)  || (printersNumber != 0) || (scannersNumber != 0) || 
			  (modemsNumber != 0) || (cdsNumber != 0)
			 )
		{
			return 0;
		}		
	}

	// check memory values for negativity and its size
	if (priority > 0 && (memoryBlock < 0  || memoryBlock > USER_MEMORY_SIZE)) 
	{
		return 0;
	}
	// check requested resources values are not more than we have available
	if (priority > 0 && (printersNumber > MAX_PRINTERS || scannersNumber > MAX_SCANNERS || 
		  								 modemsNumber > MAX_MODEMS     || cdsNumber > MAX_CDS )
		 ) 
	{
		return 0;
	}
	
	//everything is good
	return 1; 
}

/* 
 * Function: readInputFile
 * -----------------------
 *	read data from given dispatch file 
 * 
 *	fileName: name of the input file 
 *
 *	returns: a pointer to head of process control block queue(PcbPtr)   
 */
PcbPtr readInputFile(char * fileName) 
{
	//prcess details
	char buffer[MAX_INPUT_SIZE];
	int arrivalTime;
	int priority;
	int cpuTime;
	int memoryBlock;

	//to hold requested resources number
	
	int printersNumber;
	int scannersNumber;
	int modemsNumber;
	int cdsNumber;
	RsrcPtr tempResource=NULL;

	//head and tail of our process queue 
	PcbPtr queueHead = NULL;
	PcbPtr queueTail = NULL;
	PcbPtr pcbTemp = NULL;	

	FILE *fp;

	// get the input file lines, if it's more than maximum (1000) then exit
	int numberOfInputs = getTotalInputs(fileName);
	if (numberOfInputs > MAX_INPUT_LINES) 
	{
		fprintf(stderr, "Error! Number of lines are more than maximum value. Maximum is %d.\n", MAX_INPUT_LINES);
		exit(-1);
	}
	//open the input file for reading 
	if (!(fp = fopen(fileName,"r"))) 
	{
		fprintf(stderr, "Error! Dispatch file does not exist.\n");
		printf("\tUsage: ./hostd your_dispatch_file_name \n");
		exit(-1);
	}
	//loop through each line and get the values
	int line = 0;
	while(fgets(buffer,sizeof(buffer),fp) != NULL) 
	{ 
		line++;
		int numberOfValues = sscanf(buffer,"%d, %d, %d, %d, %d, %d, %d, %d", &arrivalTime, &priority, &cpuTime, &memoryBlock, &printersNumber, &scannersNumber, &modemsNumber, &cdsNumber);

		//there should be 8 comma separated values
		if (numberOfValues < 8) 
		{
			fprintf(stderr, "Error! Some lines does not have right number of values. \n");
			printf("There should be 8 comma-separated values in each line.\n");
			printf("Check your input file line: %d, and run the dispatcher again. \n",line);			
			return freeAll_pcb(queueHead);
		}
		//now check each value to be in right range 
		int correct_input = checkInputsValues(arrivalTime,priority,cpuTime,memoryBlock,printersNumber,scannersNumber,modemsNumber,cdsNumber);
		if (correct_input == 1) 
		{
			//create resources			
			tempResource = create_resource(printersNumber,scannersNumber,modemsNumber,cdsNumber);
			
			//create process
			pcbTemp = create_pcb(arrivalTime,priority,cpuTime,memoryBlock,tempResource);
			
			//if queue is empty or it only has one element, add it as usual (normal enqueue)
			if (queueHead == NULL)
			{ 
				queueHead = addToHead_pcb(queueHead,pcbTemp);
			}
			else if (queueTail == NULL) 
			{ 
				queueTail = addToHead_pcb(queueHead,pcbTemp)->next;
			}
			//otherwise queue has more than one element
			else 
			{ 
				queueTail = addToTail_pcb(queueTail,pcbTemp);
			}
		}

		//if there is any error in currect line of inputs
		else 
		{ 
			fprintf(stderr, "Error! line [%d] has a wrong format. Check your input file.\n",line);
		}
	}//end of while loop

	//close the input file 
	fclose(fp);
	
	return queueHead;
}