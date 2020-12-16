#define _CRT_SECURE_NO_WARNINGS 
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
QUEUE* create_tasks_queue(FILE* tasks_file);

//prints all the queues offset in their priority
//input: pointer to QUEUE you want to print
void PrintQueue(QUEUE* queue);

// Implementation -------------------------------------------------------

//command line: Factori.exe , tasks.txt, TasksPriorities.txt , NumberOfTasks, NumberOfthreads
int main(int argc, char* argv[]) {
	if (argc != 5) {
		printf("ERROR: there is %d arguments (need to be 5)\n", argc);
		return FUNCTION_FAILED;
	}

	QUEUE* tasks_queue = NULL;
	FILE* tasks_priorities_file = NULL;
	BOOL pass_or_fail = FALSE;

	//DEBUG start:
	int num = 1337; 
	int counter_num_of_factors = 0;
	int* prime_factor_array = NULL;
	int num_of_digits = get_number_of_digits(num);
	pass_or_fail = find_prime_factors(num, &prime_factor_array ,&counter_num_of_factors);
	
	char* string_to_file =NULL;
	create_string_to_write(&string_to_file, num, counter_num_of_factors, &prime_factor_array);

	printf("%s", string_to_file);
	free(prime_factor_array);
	free(string_to_file);
	//DEBUG end

	//read tasks proirities file and create a tasks queue
	errno_t err = fopen_s(&tasks_priorities_file,argv[2], "r");
	if (err || tasks_priorities_file == NULL) {
		printf("ERROR: failed to open the'Tasks Priorities' file");
		return FUNCTION_FAILED;
	}

	tasks_queue=create_tasks_queue(tasks_priorities_file);//crearte new queue and insert to it the priority offsets from file 
	if (tasks_queue == NULL) {
		return FUNCTION_FAILED;
	}
	//***********


	//Daniela's debug:
	PrintQueue(tasks_queue);
	ThreadFunction(tasks_queue, argv[1]);
	//end debug


	DestroyQueue(tasks_queue,&tasks_queue);//release all the memory that allocates for the queue
	fclose(tasks_priorities_file);//check if the file valid and close file
	return 0;
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

BOOL  find_prime_factors(int num, int** prime_factor_array, int* counter_num_of_factors) {

	int i = 3;
	int* p_prime_factors = NULL;
	int counter = 0;
	p_prime_factors = (int*)malloc(sizeof(int));
	if (p_prime_factors == NULL) {
		return FALSE;
	}
	while (num % 2 == 0) {
		if (p_prime_factors == NULL) {
			return FALSE;
		}
		p_prime_factors[counter] = 2;
		num = (num / 2);
		counter++;
		p_prime_factors = (int*)realloc(p_prime_factors, (counter + 1) * sizeof(int));
		if (p_prime_factors == NULL) {
			return FALSE;
		}
	}

	while (i <= sqrt(num)) {
		while (i % num) {///divide in zero ERROR
			p_prime_factors[counter] = i;
			num = (num / i);
			counter++;
			p_prime_factors = (int*)realloc(p_prime_factors, (counter + 1) * sizeof(int));
			if (p_prime_factors == NULL) {
				return FALSE;
			}
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

BOOL create_string_to_write(char** string, int num, int num_of_factors, int** factor_array) {



	int len_of_string = 27 + 9 + (5 * num_of_factors) + num_of_factors * 2;
	*string = (char*)malloc(sizeof(char) * len_of_string);
	sprintf(*string, "The prime factors of %d are:", num);
	if (*string == NULL) {
		return FALSE;
	}



	char buffer_str_factors[5];
	for (int i = 0; i < num_of_factors; i++) {

		if (i != num_of_factors - 1) {

			sprintf(buffer_str_factors, " %d,", (*factor_array)[i]);
			strcat(*string, buffer_str_factors);
		}
		else {
			sprintf(buffer_str_factors, " %d\r\n", (*factor_array)[i]);///DANIELA fixed ending
			strcat(*string, buffer_str_factors);
		}

	}
	return TRUE;
}

int get_number_of_digits(num) {
	int count = 0;
	while (num != 0) {
		count++;
		num = num /= 10;
	}
	return count;
}
