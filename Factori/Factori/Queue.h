/*this module includes all the functions and structs that belong to the queue*/


// Includes --------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <windows.h>



// Defines --------------------------------------------------------------------
#ifndef	QUEUE_H
#define	QUEUE_H

#define FUNCTION_FAILED -1


// Structs --------------------------------------------------------------------

/*this struct defiend the nodes for the  queue*/
typedef struct node {
	int offset;//the data of the node is the offset in the priority file
	struct node* next;//the next node in queue

}Node;

/*this struct defines the queue data structure*/
typedef struct Queue {
	int size;//the size of queue
	Node* head;//pointer to the first node in queue(first in priority)
	Node* tail;//pointer to the last node in queue(last in priority)
}QUEUE;

// Function Declarations -------------------------------------------------------

/*this function creats new queue
if succeed the function return a new pointer to an empty queue
if fails returns NULL*/
QUEUE* InitializeQueue();

/*this function return the offset of the first node that represt the offset of the most priority mission*/
/*input: pointer to queue*/
int Top(QUEUE* queue);

/*this function return the offset of the first node that represt the offset of the most priority mission and remove it from the queue*/
/*input: pointer to queue*/
int Pop(QUEUE* queue);

/*this function push a new offset the the end of queue by creating a new node and link is to the end of queue
return 0 if the push succeed
return -1 if it failed*/
int Push(QUEUE* queue,//pointer to queue
	int offset);// the new offset that will be insert to the end of queue

//this function get a pointer to queue and check if it empty
//return true if it empty, otherwise return false.
BOOL Empty(QUEUE* queue);

//this funtion release all the memory that allocated to the queue including the queue, and setting the the queue pointer to NULL
/*input: pointer to queue*/
void DestroyQueue(QUEUE* queue);

//this function creats a new node that contain the given offset
//input: int offset that will be in the new node
//if succeed it return pointer to new node, otherwise it return NULL
Node* CreateNode(int offset);
#endif 
#pragma once
