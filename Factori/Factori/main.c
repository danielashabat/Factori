
// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Queue.h"

// Defines --------------------------------------------------------------------

#define FUNCTION_FAILED -1
#define _CRT_SECURE_NO_WARNINGS
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
	BOOL pass_or_fail = FALSE;
	int num = 24; 
	int counter_num_of_factors = 0;
	int* prime_factor_array = NULL;
	int num_of_digits = get_number_of_digits(num);
	pass_or_fail = find_prime_factors(num, &prime_factor_array ,&counter_num_of_factors);
	
	char* string_to_file =NULL;
	create_string_to_write(&string_to_file, num, counter_num_of_factors, &prime_factor_array);
	for (int i = 0; i < counter_num_of_factors; i++) {
		printf("%d\n", prime_factor_array[i]);
	}
	free(prime_factor_array);
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

BOOL  find_prime_factors(int num , int** prime_factor_array , int* counter_num_of_factors) {
	
	int i = 3;
	int* p_prime_factors = NULL;
	int counter = 0;
	p_prime_factors = (int*)malloc(sizeof(int));
	
	while (num % 2 == 0) {
		p_prime_factors[counter] = 2;
		num = (num / 2);
		counter++;
		p_prime_factors = (int*)realloc(p_prime_factors, (counter +1) * sizeof(int));
	}

	while (i <= sqrt(num)) {
		while (i % num) {
			p_prime_factors[counter] = i;
			num = (num / i);
			counter++;
			p_prime_factors = (int*)realloc(p_prime_factors, (counter + 1) * sizeof(int));
		}
		i = i + 2;
	}
	if (num > 2) {
		(p_prime_factors)[counter] = i;
		counter++;
	}
	*counter_num_of_factors = counter;
	*prime_factor_array = p_prime_factors;
	return TRUE;
}

BOOL create_string_to_write(char** string, int num, int num_of_factors , int** factor_array) {
	
	char* str1 = "The prime factors of ";
	char* str2 = " are:";
	char** buffer_str_factors = NULL;
	int num_of_digits = get_number_of_digits(num);
	char* buffer_str_num = (char*)malloc(sizeof(char)*(num_of_digits + 26));
	sprintf(buffer_str_num, "The prime factors of %d are:", num);
	int len_of_string = 27 + num_of_digits + (num_of_factors * 3);
	*string = (char*)malloc(sizeof(char) * len_of_string);
	
	strcpy_s(*string,sizeof(*string), str1);
	strcpy_s(*string, sizeof(*string), buffer_str_num);
	strcpy_s(*string, sizeof(*string), str2);
	free(buffer_str_num);
	buffer_str_factors = (char**)malloc(sizeof(char*) * num_of_factors);
	for (int i = 0; i < num_of_factors; i++) {
		strcpy_s(*string, sizeof(*string), " ");
		num_of_digits = get_number_of_digits(num);
		buffer_str_factors[i] = (char*)malloc(sizeof(char) * num_of_digits);
		sprintf(buffer_str_factors[i], "%d", num);
		strcpy_s(*string, sizeof(*string), buffer_str_factors[i]);
		if (i != num_of_factors) {
			strcpy_s(*string, sizeof(*string), ",");
		}
		else {
			strcpy_s(*string, sizeof(*string), "\n\r");
		}
		free(buffer_str_factors[i]);
	}
	free(buffer_str_factors);
}

int get_number_of_digits(num) {
	int count = 0;
	while (num != 0) {
		count++;
		num = num /= 10;
	}
}
