/*this module includes all the functions and structs that belong to the Lock*/

#ifndef	LOCK_H
#define	LOCK_H

// Includes --------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <windows.h>



// Defines --------------------------------------------------------------------
#define TIME_OUT 5000//5 seconds
// Structs --------------------------------------------------------------------

/*this struct defiend the nodes for the  queue*/
typedef struct lock {
	int readers;//the number of threads are reading at a time
	HANDLE mutex;
	HANDLE semaphore;
}Lock;

// Function Declarations -------------------------------------------------------

/*creating new lock data structure and initial all it values.
if succeed return pointer to a new 'Lock'
if failed, return NULL*/
Lock* InitializeLock();

/*check if read operation is available, if it available, the function blocking writing operation
input: ptr to lock
return values: if TRUE the function locked the file from writing and only reading mode is available,
if FALSE the function failed locking file for reading (the file is in writing mode)*/
BOOL read_lock(Lock* lock);

/*this function release the file from reading mode and it is now available to any mode*/
/*input: ptr to lock
return values : if TRUE the function succeed releasing the lock from reading ,
if FALSE the function occured an error and failed */
BOOL read_release(Lock* lock);
#endif 