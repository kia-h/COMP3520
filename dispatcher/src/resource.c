/**
 * COMP3520 - Operating Systems - Assignment 2
 * 
 * author: Internet's own boy
 */

#include <stdio.h>
#include <stdlib.h>

#include "../header/resource.h"
#include "../header/memoryblock.h"

/* 
 * Function: check_resource
 * ------------------------
 *	checks if resource(s) is/are available for requesting process
 * 
 *	availableResource: list of available resources
 *	requestedProcess : requested resurce(s)by a process
 *	
 *	returns: a pointer to updated available resources list  
 */
int check_resource(RsrcPtr availableResource, RsrcPtr requestedResources)
{
	
	//check for our 4 types of resources (printers, scanners, modems, and cds)
	if(availableResource->printers >= requestedResources->printers && 
		 availableResource->scanners >= requestedResources->scanners && 
	   availableResource->modems   >= requestedResources->modems   && 
	   availableResource->cds      >= requestedResources->cds ) 
	{
		//yes they are
		return 1; 
	}
	//no they are not
	return 0; 
}

/* 
 * Function: create_resource 
 * -------------------------
 *	creates resource for our 4 types of available resources
 * 
 *	printers: number of available priters 
 *	scanners: number of available scanners
 *	modems  : number of available modems
 *  cds     : number of available cds and return the address
 *	
 *	returns: a pointer to newly created resources 
 */
RsrcPtr create_resource(int printers, int scanners, int modems, int cds)
{
	//get the memeory and check if it was successful
	RsrcPtr temp = (RsrcPtr)malloc(sizeof(Rsrc));
	
	//check memory allocation ... from memoryblock.h
	checkMalloc((void*) temp);

	temp->printers = printers;
	temp->scanners = scanners;
	temp->modems = modems;
	temp->cds = cds;
	return temp;
}

/* 
 * Function: allocate_resource
 * ---------------------------
 *	allocate requested resources by a processcreates resource for our 4 types of available resources
 * 
 *	availableResource: list of available resources
 *	requestedProcess : requested resource(s) by a process
 *	
 *	returns: a pointer to updated available resources list  
 */
RsrcPtr allocate_resource(RsrcPtr availableResource, RsrcPtr requestedProcess)
{
	availableResource->printers = availableResource->printers - requestedProcess->printers;
	availableResource->scanners = availableResource->scanners - requestedProcess->scanners;
	availableResource->modems   = availableResource->modems   - requestedProcess->modems;
	availableResource->cds      = availableResource->cds      - requestedProcess->cds;
	return availableResource;
}

/* 
 * Function: update_resource
 * -------------------------
 *	updates the number of available resource.i.e requested process doesn't need resources anymore
 * 
 *	availableResource: list of available resources
 *	requestedProcess : requested resurce(s)by a process
 *	
 *	returns: a pointer to updated available resources list  
 */
RsrcPtr claim_resource(RsrcPtr availableResource, RsrcPtr claim)
{
	//update the number of resources availability
	availableResource->printers = availableResource->printers + claim->printers;
	availableResource->scanners = availableResource->scanners + claim->scanners;
	availableResource->modems   = availableResource->modems   + claim->modems;
	availableResource->cds      = availableResource->cds      + claim->cds;
	return availableResource;
}

/* 
 * Function: free_resource
 * -----------------------
 *	makes used memory free
 * 
 *	inUsedResource: list of currently in used resources
 *	
 *	returns: a pointer to updated available resources list  
 */
void free_resource(RsrcPtr inUsedResource) 
{
	if (inUsedResource) 
	{
		free(inUsedResource);
	}
}
