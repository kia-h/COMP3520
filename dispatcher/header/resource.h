/**
 * COMP3520 - Operating Systems - Assignment 2 
 * 
 * author: Internet's own boy
 */

#ifndef RESOURCE_H
#define RESOURCE_H

/*******************************************************************

  Assignment 2 - HOST dispatcher

    resource.h - process resource allocation functions for HOST dispatcher

		int check_resource(RsrcPtr , RsrcPtr );					 - check resources availability 
		RsrcPtr create_resource(int , int , int , int ); - create resources
		RsrcPtr allocate_resource(RsrcPtr , RsrcPtr );		 - allocate resources 
		RsrcPtr claim_resource(RsrcPtr , RsrcPtr );			 - update the available resources number
		void free_resource(RsrcPtr );										 - free resources

    see resource.c for fuller description of function arguments and returns
		p.s. some text taken from course provided sample code like this box style.
		     this header structure has been provided by course teaching staff,
	       however i changed it. so there might be some similarities of course
********************************************************************/

/******* resource management definitions*******/

//maximum number of available resources
#define MAX_PRINTERS 2
#define MAX_SCANNERS 1
#define MAX_MODEMS   1
#define MAX_CDS	     2

struct resource 
{
	int printers;
	int scanners;
	int modems;
	int cds;
};

typedef struct resource Rsrc;
typedef struct resource * RsrcPtr;

/******* resource management function prototypes *******/

int     check_resource    (RsrcPtr , RsrcPtr );
RsrcPtr create_resource   (int , int , int , int );
RsrcPtr allocate_resource (RsrcPtr , RsrcPtr );
RsrcPtr claim_resource   (RsrcPtr , RsrcPtr );

void    free_resource     (RsrcPtr );

#endif