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
/*read the line that start with the offset that we got from queue
input: handle to file,ptr to int so we can save num ,offset in file
return values: if TRUE the function succussed to read the num,
if FALSE the function failed at reading */
BOOL read_num_from_tasks_file(HANDLE hfile_tasks, int* num, LONG offset);
/*write the string of the prime factors to the kast line in the file
input: handle to file,string 
return values: if TRUE the function succussed to write the string,
if FALSE the function failed at writing */
BOOL write_to_tasks_file(HANDLE hfile_tasks, char* string_to_file);
/*Thread function - this the unction every thread runs.
1.find the offset from queue - using mutex to pritect access
2. read from file using lock
3. analyze the prime factors and create string 
4. write the string into the file 
input: lpParam -> task file name,mutux to queue,pte to queue,ptr to lock
return values: if TRUE the function succussed to read and write,
if FALSE the function failed  */
DWORD WINAPI ThreadFunction(LPVOID lpParam);
//the purpose of this function is to create data for passsing the necessary arguments to 'ThreadFunction'
//it returns pointer to ThreadData that contains the relevant data per thread by it's index thread.
//if the function fails it returns FALSE
BOOL Create_Thread_data(char tasks_file_path[], QUEUE* task_queue, Lock* lock, HANDLE queue_mutex, ThreadData** ptr_to_thread_data);
/*get task from queue if the queue is empty or not and get the offset . We protect the acess to queue with mutex 
input: handle to queue mutex,ptr to task queue, point to int to save the pffset, ptr to Bool to save if ampy or not
return values: if TRUE the function succussed to get the offset corectly and to lock and release the mutex ,
if FALSE the function failed */
BOOL check_queue_status(HANDLE queue_mutex_handle, QUEUE* tasks_queue, int* task_offset, BOOL* queue_is_empty);
/*get all the prime factor of some factors and get the num to factors.
input: integer num , ptr array to save the frime factors, ptr to integer who saves the num pf factors 
return values: if TRUE the function succussed to save all factors ,
if FALSE the function failed */
BOOL  find_prime_factors(int num, int** prime_factor_array, int* counter_num_of_factors);
/*combine all the factors into one string
input:ptr to string to save the new string, num , number of factors , ptr_ to the array we save the factors at
return values: if TRUE the function succussed create string ,
if FALSE the function failed */
BOOL create_string_to_write(char** string, int num, int num_of_factors, int** factor_array);
//functions for handling errors:
/*check that the handle to the file created sucssessfully*/
BOOL check_file_handle(HANDLE h_file, char* file_name);
/*check that return value from Read file and WriteFile is not fail */
BOOL check_ReadFile_WriteFile(BOOL bErrorFlag, DWORD number_of_bytes_to_read_or_write, DWORD lpNumberOfBytesRead_or_Written);
/*get all the paramters the we need to close or free  before we exit the thread if we fail*/
int error_close_thread_handles(int* prime_factor_array, char* string_to_file, HANDLE hfile);
#endif