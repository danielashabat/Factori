
// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Queue.h"

// Defines --------------------------------------------------------------------

#define FUNCTION_FAILED -1

// Function Declarations -------------------------------------------------------

//this function create a new queue and insert the offsets from the 'Tasks Priorities' file by creating a new nodes
//return pointer to QUEUE if succeed, otherwise return NULL
//input: pointer to 'Tasks Priorities' FILE
QUEUE* create_queue_tasks(FILE* tasks_file);

// Implementation -------------------------------------------------------

//command line: Factori.exe , tasks.txt, TasksPriorities.txt , NumberOfTasks, NumberOfthreads
int main(int argc, char* argv[]) {
	//check argc 

	QUEUE* queue_tasks = NULL;
	FILE* tasks_file = NULL;


	errno_t err = fopen_s(&tasks_file,argv[1], "r");
	if (err || tasks_file == NULL) {
		printf("ERROR: failed to open the'Tasks Priorities' file");
		return FUNCTION_FAILED;
	}

	queue_tasks=create_queue_tasks(tasks_file);//crearte new queue and insert to it the priority offsets from file 
	if (queue_tasks == NULL) {
		return FUNCTION_FAILED;
	}
	PrintQueue(queue_tasks);
	DestroyQueue(queue_tasks);//release all the memory that allocates for the queue
	fclose(tasks_file);//check if the file valid and close file
	return 0;
}

QUEUE* create_queue_tasks(FILE* tasks_file) {
	QUEUE* queue_tasks = NULL;
	queue_tasks=InitializeQueue();//create new and empty queue
	int offset;
	if (queue_tasks != NULL) {//check if intialize succeed
		while (!feof(tasks_file)) {
			fscanf_s(tasks_file, "%d\r\n", &offset);
			Push(queue_tasks, offset);//pushing the new 'offset' to the end of the queue
		}
	}
	return queue_tasks;//return the new queue
}

BOOL  find_prime_factors(int num , int* prime_factor_array) {
	
	int counter = 0;
	int i = 3;
	prime_factor_array = (int*)malloc(sizeof(int));
	while (num % 2 == 0) {
		prime_factor_array[counter] = 2;
		num = (num / 2);
		counter++;
		prime_factor_array = (int*)realloc(prime_factor_array,counter*sizeof(int))
	}

	while (i <= sqrt(num)) {
		while (i % num) {
			prime_factor_array[counter] = i;
			num = (num/i)
			counter++;
			prime_factor_array = (int*)realloc(prime_factor_array, counter * sizeof(int))
			
		}
	}
}

