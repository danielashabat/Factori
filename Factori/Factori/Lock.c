// Includes --------------------------------------------------------------------

#include "Lock.h"

// Implementation -------------------------------------------------------

/*if succeed return pointer to a new 'Lock'
if failed, return NULL*/
Lock* InitializeLock() {
	Lock* lock = NULL;
	lock = (Lock*)malloc(sizeof(Lock));
	if (NULL == lock) {
		printf("ERROR: memory allocation for lock failed!\n");
		return NULL;
	}
	//initial values
	lock->readers = 0;
	lock->semaphore = NULL;
	lock->mutex = NULL;

	/////create new semaphore
	lock->semaphore = CreateSemaphore(
		NULL,	/* Default security attributes */
		1,		/* Initial Count*/
		1,		/* Maximum Count */
		NULL);  /* un-named */

	if (lock->semaphore == NULL)
	{
		printf("-ERROR: %d - create semaphore failed!\n", GetLastError());
		return NULL;
	}

	/////create new mutex
	lock->mutex = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL);  /* un-named */
	if (lock->mutex == NULL)
	{
		printf("-ERROR: %d - create mutex failed!\n", GetLastError());
		return NULL;
	}
	return lock;//if function succeed return pointer to new lock
}

BOOL read_lock(Lock* lock ,int time_out) {
	DWORD wait_res;
	BOOL release_res;
	wait_res = WaitForSingleObject(lock->mutex, time_out);//lock mutex
	if (wait_res != WAIT_OBJECT_0) {
		printf("-ERROR: %d - WaitForSingleObject failed !\n", GetLastError());
		return FALSE;
	}
	lock->readers++;//increase the number of readers
	if (lock->readers == 1) {//if it the first reader
		wait_res = WaitForSingleObject(lock->semaphore, time_out);//down to semaphore -> writing operation will be block
		if (wait_res != WAIT_OBJECT_0) {
			printf("-ERROR: %d - WaitForSingleObject failed !\n", GetLastError());
			return FALSE;
		}
	}

	release_res = ReleaseMutex(lock->mutex);//release lock on mutex
	if (release_res == FALSE) {
		printf("-ERROR: %d -release mutex failed !\n", GetLastError());
		return FALSE;
	}
	printf("READ lock success!\n");
	return TRUE;//function succeed
}


BOOL read_release(Lock* lock , int time_out) {
	DWORD wait_res;
	BOOL release_res;
	wait_res = WaitForSingleObject(lock->mutex, time_out);//lock mutex
	if (wait_res != WAIT_OBJECT_0) {
		printf("-ERROR: %d - WaitForSingleObject failed !\n", GetLastError());
		return FALSE;
	}
	lock->readers--;//decrease the number of readers
	if (lock->readers == 0) {//if it was the last reader
		release_res = ReleaseSemaphore(lock->semaphore,1,NULL);//+1 up to semaphore ->writing operation is available
		if (release_res == FALSE) {
			printf("-ERROR: %d - release semaphore failed !\n", GetLastError());
			return FALSE;
		}
	}
	release_res = ReleaseMutex(lock->mutex);//release lock on mutex
	if (release_res == FALSE) {
		printf("-ERROR: %d -release mutex failed !\n", GetLastError());
		return FALSE;
	}
	printf("READ release success!\n");
	return TRUE;//function succeed
}

BOOL  write_lock(Lock* lock, int time_out) {
	DWORD wait_res;

	wait_res = WaitForSingleObject(lock->semaphore, time_out);//down to semaphore -> write and read operations are not available for other threads
	if (wait_res != WAIT_OBJECT_0) {
		printf("-ERROR: %d - WaitForSingleObject failed !\n", GetLastError());
		return FALSE;
	}
	printf("WRITE lock success!\n");
	return TRUE;
}

BOOL  write_release(Lock* lock, int time_out) {
	BOOL release_res;
	release_res = ReleaseSemaphore(lock->semaphore, 1, NULL);//+1 up to semaphore ->write or read operations is now available for other threads
	if (release_res == FALSE) {
		printf("-ERROR: %d - release semaphore failed !\n", GetLastError());
		return FALSE;
	}
	printf("WRITE release success!\n");
	return TRUE;
}