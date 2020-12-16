/*this module includes all the functions and structs that belong to the threads*/

#ifndef THREADSFUNCTIONS_H
#define	THREADSFUNCTIONS_H

// Includes --------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <windows.h>

#include "Lock.h"
#include "Queue.h"

// Defines --------------------------------------------------------------------
#define SUCCSESS TRUE
#define FAIL FALSE

#define IF_ERROR_FOUND_END_PROGRAM(hfile_tasks,tasks_file_name) if (!check_file_handle(hfile_tasks, tasks_file_name)) {\
	CloseHandle(hfile_tasks);\
	return FAIL;}

#define IF_FAILED_END_PROGRAM(result) if (result ==FAIL) {\
	CloseHandle(hfile_tasks);\
	return FAIL;}
// VARIABELS --------------------------------------------------------------------


// Structs --------------------------------------------------------------------


// Function Declarations -------------------------------------------------------
BOOL read_num_from_tasks_file(HANDLE hfile_tasks, int* num, LONG offset);
BOOL write_to_tasks_file(HANDLE hfile_tasks, int num);

//functions for handling errors:
BOOL check_file_handle(HANDLE h_file, char* file_name);
BOOL check_ReadFile_WriteFile(BOOL bErrorFlag, DWORD number_of_bytes_to_read_or_write, DWORD lpNumberOfBytesRead_or_Written);

#endif