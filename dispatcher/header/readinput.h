/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#ifndef READINPUT_H
#define READINPUT_H

/*******************************************************************

   Assignment 2 - HOST dispatcher

  readinput.h - read inputs functions for HOST dispatcher

	int getTotalInputs(char* ) ;    - get total number of lines in input file
	PcbPtr readInputFile(char * );	- read inputs from file
	int checkInputsValues(int , int , int , int , int , int , int , int ); - check input values
	
  see readinput.c for fuller description of function arguments and returns
	p.s. some text taken from course provided sample code like this box style
 ********************************************************************/

#include  "processblock.h"

//maximum number of lines based on assignment spec
#define MAX_INPUT_LINES 1000

/******* read inputs function prototypes *******/

int    getTotalInputs		(char* ) ;
PcbPtr readInputFile		(char * );
int    checkInputsValues(int , int , int , int , int , int , int , int );

#endif