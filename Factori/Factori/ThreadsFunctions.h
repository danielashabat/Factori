/*this module includes all the functions and structs that belong to the threads*/

#ifndef THREADSFUNCTIONS_H
#define	THREADSFUNCTIONS_H

// Includes --------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <windows.h>
#include <math.h>

#include "Lock.h"
#include "Queue.h"

// Defines --------------------------------------------------------------------
#define SUCCSESS TRUE
#define FAIL FALSE

#define IF_FAILED_END_PROGRAM(result) if (result ==FAIL) {\
	CloseHandle(hfile_tasks);\
	return FAIL;}
// VARIABELS --------------------------------------------------------------------


// Structs --------------------------------------------------------------------
typedef struct ThreadData {
	char input_path[MAX_PATH];// the path of the input file
	QUEUE* task_queue;
	Lock* lock  ;//the statring point (by bytes) the thread need to decrypte/encrypte 
	HANDLE queue_mutex;
} ThreadData;

// Function Declarations -------------------------------------------------------
BOOL read_num_from_tasks_file(HANDLE hfile_tasks, int* num, LONG offset);
BOOL write_to_tasks_file(HANDLE hfile_tasks, char* string_to_file);
DWORD WINAPI ThreadFunction(LPVOID lpParam);
BOOL Create_Thread_data(char tasks_file_path[], QUEUE* task_queue, Lock* lock, HANDLE queue_mutex, ThreadData** ptr_to_thread_data);
BOOL check_queue_status(HANDLE queue_mutex_handle, QUEUE* tasks_queue, int* task_offset, BOOL* queue_is_empty);
BOOL  find_prime_factors(int num, int** prime_factor_array, int* counter_num_of_factors);
BOOL create_string_to_write(char** string, int num, int num_of_factors, int** factor_array);
//functions for handling errors:
BOOL check_file_handle(HANDLE h_file, char* file_name);
BOOL check_ReadFile_WriteFile(BOOL bErrorFlag, DWORD number_of_bytes_to_read_or_write, DWORD lpNumberOfBytesRead_or_Written);

#endif