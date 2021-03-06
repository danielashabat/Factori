/*Authors -Daniela Shabat 316415645, Anat Sinai 312578149.
Project � Factori (exersice 3 in System Operation course)
Description � in this project the program we practice the mutex/semaphore concepts with threads */

// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "Queue.h"
#include "Lock.h"
#include "ThreadsFunctions.h"

// Defines --------------------------------------------------------------------

#define FUNCTION_FAILED -1
#define THREAD_SUCCESS ((int)(0))
#define THREAD_FAIL ((int)(-1))

#define TIMEOUT 5000 // 5 seconds is  max time for one task

// Function Declarations -------------------------------------------------------

//this function create a new queue and insert the offsets from the 'Tasks Priorities' file by creating a new nodes
//return pointer to QUEUE if succeed, otherwise return NULL
//input: pointer to 'Tasks Priorities' FILE
QUEUE* create_tasks_queue(FILE* tasks_file);

//prints all the queues offset in their priority
//input: pointer to QUEUE you want to print
void PrintQueue(QUEUE* queue);

BOOL initalize_queue_mutex(HANDLE* queue_mutex);
BOOL check_thread_exit(int num_threads, HANDLE* threads_handles, int num_of_lines);
BOOL  CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,LPVOID p_thread_parameters,LPDWORD p_thread_id, HANDLE* thread_handle);
BOOL close_program_clean(QUEUE* tasks_queue,
	FILE* tasks_priorities_file,
	Lock* lock,
	HANDLE queue_mutex,
	HANDLE* threads_handles,
	DWORD* thread_ids,
	ThreadData* ptr_to_thread_data, int num_of_thread, BOOL pass_or_fail);
// Implementation -------------------------------------------------------

//command line: Factori.exe , Tasks.txt, TasksPriorities.txt , NumberOfTasks, NumberOfthreads
int main(int argc, char* argv[]) {
	if (argc != 5) {
		printf("ERROR: there is %d arguments (need to be 5)\n", argc);
		return FUNCTION_FAILED;
	}

	QUEUE* tasks_queue = NULL;
	FILE* tasks_priorities_file = NULL;
	BOOL pass_or_fail = FALSE;
	Lock* lock = NULL;
	HANDLE queue_mutex = NULL;
	int num_of_thread;
	int num_of_lines;
	HANDLE* threads_handles = NULL; //pointer to threads handles array
	DWORD* thread_ids = NULL; ////pointer to threads ids array
	ThreadData* ptr_to_thread_data = NULL;
	char tasks_file_name[MAX_PATH];


	//time vars
	time_t start_time;//in seconds unit
	time_t end_time;//in seconds unit

	start_time = time(NULL);
	//read tasks proirities file and create a tasks queue
	errno_t err = fopen_s(&tasks_priorities_file,argv[2], "r");
	if (err || tasks_priorities_file == NULL) {
		printf("ERROR: failed to open the'Tasks Priorities' file");
		return FUNCTION_FAILED;
	}
	strcpy_s(tasks_file_name, MAX_PATH, argv[1]);
	num_of_lines = atoi(argv[3]);
	num_of_thread = atoi(argv[4]);
	tasks_queue=create_tasks_queue(tasks_priorities_file);//crearte new queue and insert to it the priority offsets from file 
	if (tasks_queue == NULL) {
		return FUNCTION_FAILED;
	}
	PrintQueue(tasks_queue);
	//***********
	lock = InitializeLock();//initial the lock ptr
	if (lock == NULL) {
		return (close_program_clean(tasks_queue, tasks_priorities_file, lock, queue_mutex, threads_handles, thread_ids,
			ptr_to_thread_data, num_of_thread, FALSE));
			
	}
	pass_or_fail = initalize_queue_mutex(&queue_mutex);//initial the queue mutex 
	if (!pass_or_fail) return (close_program_clean(tasks_queue, tasks_priorities_file,  lock, queue_mutex, threads_handles, thread_ids,
		ptr_to_thread_data, num_of_thread, pass_or_fail));
	pass_or_fail = Create_Thread_data(tasks_file_name, tasks_queue, lock, queue_mutex, &ptr_to_thread_data);//create the pinter that we give to the thread
	if (!pass_or_fail) return (close_program_clean(tasks_queue, tasks_priorities_file,  lock, queue_mutex, threads_handles, thread_ids,
		ptr_to_thread_data, num_of_thread, pass_or_fail));
	if(num_of_thread!=0) threads_handles = (HANDLE*)malloc(sizeof(HANDLE) * num_of_thread);//creating array of handles in the size of num_threads

	
	if (threads_handles == NULL) {//checl if aloocation failed
		pass_or_fail = FAIL;
		if (!pass_or_fail) return (close_program_clean(tasks_queue, tasks_priorities_file, lock, queue_mutex, threads_handles, thread_ids,
			ptr_to_thread_data, num_of_thread, pass_or_fail));
	}

	for (int i = 0; i < num_of_thread; i++) {//inital array to NULL for close handles
		threads_handles[i] = NULL;
	}

	thread_ids = (DWORD*)malloc(num_of_thread * sizeof(DWORD));// creating array of DWORD in the size of num_threads

	if (thread_ids == NULL) {//check if aloocation failed
		pass_or_fail = FAIL;
		printf("memory allocation failed\n");
		if (!pass_or_fail) return (close_program_clean(tasks_queue, tasks_priorities_file, lock, queue_mutex, threads_handles, thread_ids,
			ptr_to_thread_data, num_of_thread, pass_or_fail));
	}

	

	//create_threads
	for (int i = 0; i < num_of_thread; i++)
	{
		pass_or_fail = CreateThreadSimple(ThreadFunction, ptr_to_thread_data, &(thread_ids[i]),(threads_handles+i));
		if (NULL == threads_handles[i])
		{
			printf("Error when creating thread: %d\n", GetLastError());
			pass_or_fail = FAIL;
			if (!pass_or_fail) return (close_program_clean(tasks_queue, tasks_priorities_file, lock, queue_mutex, threads_handles, thread_ids,
				ptr_to_thread_data, num_of_thread, pass_or_fail));
		}
	}
	//wait for the last thread to finish and check that the return value is as expected
	pass_or_fail = check_thread_exit(num_of_thread, threads_handles,num_of_lines);
	if (!pass_or_fail) return (close_program_clean(tasks_queue, tasks_priorities_file, lock, queue_mutex, threads_handles, thread_ids,
		ptr_to_thread_data, num_of_thread, pass_or_fail));
	
	end_time = time(NULL);
	printf("this function ran in %d seconds\n", (int)(end_time - start_time));
	return (close_program_clean(tasks_queue, tasks_priorities_file, lock, queue_mutex, threads_handles, thread_ids,
		ptr_to_thread_data, num_of_thread, TRUE));
	
	
}

BOOL close_program_clean(QUEUE* tasks_queue,
		FILE* tasks_priorities_file,
		Lock* lock ,
		HANDLE queue_mutex ,
		HANDLE* threads_handles, 
		DWORD* thread_ids, 
		ThreadData* ptr_to_thread_data,int num_of_thread,BOOL pass_or_fail ){
	DWORD retval = 1;
	BOOL pass_or_fail_clean_program= TRUE;
	BOOL queue_mutex_free_flag = FALSE;
	//close all thread handles
	for (int i = 0; i < num_of_thread; i++) {
		if (threads_handles[i] != NULL) retval = CloseHandle(threads_handles[i]);
		if (0 == retval)
		{
			printf("Error when closing thread: %d\n", GetLastError());
			pass_or_fail_clean_program = FALSE;
		}
	}
	
	if (thread_ids != NULL) free(thread_ids);

	if (lock != NULL)  pass_or_fail_clean_program = DestroyLock(lock, &lock);

	if (tasks_queue != NULL)  DestroyQueue(tasks_queue, &tasks_queue);

	if (queue_mutex != NULL) {
		CloseHandle(queue_mutex);
	}
	
	if (ptr_to_thread_data!= NULL) {
		free(ptr_to_thread_data);
	}
	fclose(tasks_priorities_file);
	
	if (!pass_or_fail_clean_program || !pass_or_fail) return FUNCTION_FAILED;
	else return 0; 

}





QUEUE* create_tasks_queue(FILE* tasks_file) {
	QUEUE* tasks_queue = NULL;
	tasks_queue=InitializeQueue();//create new and empty queue
	int offset;
	if (tasks_queue != NULL) {//check if intialize succeed
		while (!feof(tasks_file)) {
			fscanf_s(tasks_file, "%d\r\n", &offset);
			Push(tasks_queue, offset);//pushing the new 'offset' to the end of the queue
			
		}
	}
	return tasks_queue;//return the new queue
}

void PrintQueue(QUEUE* queue) {
	if (queue == NULL) {
		printf("ERROR:queue pointer is not valid!");
		return;
	}
	if (Empty(queue)) {
		printf("the queue is empty!\n");
		return;
	}
	Node* curr_node = queue->head;
	printf("the queue contains now:(from first priority to last)\n");
	while (curr_node != NULL) {
		printf("%d\n", curr_node->offset);
		curr_node = curr_node->next;
	}
}




BOOL initalize_queue_mutex(HANDLE* queue_mutex) {
	*queue_mutex = CreateMutex(
		NULL,	/* default security attributes */
		FALSE,	/* initially not owned */
		NULL);	/* unnamed mutex */
	if (NULL == *queue_mutex)
	{
		printf("Error when creating mutex: %d\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}



BOOL  CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPVOID p_thread_parameters,
	LPDWORD p_thread_id,HANDLE* thread_handle)
{
	

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		return FALSE;
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		return FALSE;
	}

	*thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		p_start_routine,     /*  thread function */
		p_thread_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		p_thread_id);        /*  returns the thread identifier */

	if (NULL == *thread_handle)
	{
		printf("Couldn't create thread\n");
		return FALSE;
	}

	return TRUE;
}


BOOL check_thread_exit(int num_threads,HANDLE* threads_handles, int num_of_lines) {
	DWORD dw_ret = 0;
	BOOL pass_or_fail = TRUE;
	DWORD exit_code;
	BOOL ret_val;
	int tasks = num_of_lines;//add one for safety

	dw_ret = WaitForMultipleObjects(
		num_threads,  // number of objects in array
		threads_handles,     // array of objects
		TRUE,       // wait for all objects to be signaled
		tasks*TIMEOUT);       

	// check for failure 
	if (dw_ret != WAIT_OBJECT_0) {
		printf("ERROR:%d while waiting to threads to finish\n", GetLastError());
		pass_or_fail = FAIL;
		
		
	}
	//check exit code of each thread
	for (int i = 0; i < num_threads; i++) {
		ret_val = GetExitCodeThread(*(threads_handles + i), &exit_code);
		if (ret_val == 0) {
			printf("Error when getting thread exit code\n");
			pass_or_fail = FAIL;
		}
		if (THREAD_SUCCESS != (int)exit_code) {
			pass_or_fail = FAIL;
			
		}

	}
	return pass_or_fail;
}
