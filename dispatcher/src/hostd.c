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
#include "../header/readinput.h"
#include "../header/dispatcher.h"
#include "../header/memoryblock.h"

int main(int argc, char const *argv[])
{
	// to hold inputs from dispatch file
	PcbPtr inputQueue; 

	// check for dispatch file, if it has been given as the run time argument
  if(argc > 2) 
  { 
    printf("Too many arguments! Please specify one input file only\n");
    printf("Usage: ./hostd your_dispatch_file_name \n");
    exit(1);
  } 
	else if (argc==1) 
	{
		printf("Please specify input file name\n");
		printf("Usage: ./hostd your_dispatch_file_name \n");
		exit(1);
	} 
	
	//allocate memory and check for outcome
	char * fileName = (char*)malloc(MAX_INPUT_SIZE);
	checkMalloc((void*)fileName);

	strcpy(fileName,argv[1]);

	// read the file and put them in the queue 
	inputQueue = readInputFile(fileName); 

	//check for error in reading the input file
	if (inputQueue == NULL) 
	{ 
		exit(-1);
	}

	// everything's good, so schedule the jobs
	dispatcher(inputQueue); 
	return 0;
}