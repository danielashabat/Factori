// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Queue.h"

// Implementation -------------------------------------------------------

QUEUE* InitializeQueue() {
    QUEUE* queue = (QUEUE*)malloc(sizeof(QUEUE));
    if (queue == NULL) {
        return NULL;
    }

    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

Node* CreateNode(int offset) {
        Node* node_ptr = (Node*)malloc(sizeof(Node));
        if (node_ptr != NULL)
        {
            node_ptr->offset = offset;
            node_ptr->next = NULL;
        }
        return node_ptr;
}


int Top(QUEUE* queue) {
    if (Empty(queue)) {
        printf("ERROR: Top function failed because queue is empty\n");
        return FUNCTION_FAILED;
    }
    //if not empty
    return (queue->head)->offset;
}

int Pop(QUEUE* queue) {
    if (Empty(queue)) {
        printf("ERROR: Pop function failed because queue is empty\n");
        return FUNCTION_FAILED;
    }
    //if not empty
    Node* first = queue->head;
    int offset = first->offset;

    if (queue->size == 1) {//when there is only 1 node the head and tail contains the same node
        queue->tail = NULL;
    }

    queue->head = first->next;
    queue->size--;

    free(first);
    return offset;
}

int Push(QUEUE* queue, int offset) {
    Node* new_node = CreateNode(offset);
    if (new_node == NULL)
    {
        printf("Fatal error: unable to allocate memory!\n");
        return FUNCTION_FAILED;//NEED TO TERMINATE PROGRAM
    }
    if (Empty(queue)) {
        queue->head = new_node;//the first node is head and tail
        queue->tail = new_node;
        queue->size++;//increase queue's size by 1
    }
    else {//if queue is not empty
        (queue->tail)->next = new_node; //link the last node in the queue to the new node
        queue->tail = new_node;//updating the tail in queue to the new node
        queue->size++;//increase queue's size by 1
    }
    return 0;//the function succeed
}

BOOL Empty(QUEUE* queue) {
    if (queue != NULL) {//check the pointer is valid
        if (queue->size == 0)
            return true;
        else
            return false;
    }
    printf("ERROR:queue pointer is not valid!");
    return FUNCTION_FAILED;
}

void DestroyQueue(QUEUE* queue) {
    if (!Empty(queue)) {//if queue is not empty, release all nodes
        Node* curr_node = queue->head;
        Node* temp = NULL;

        while (curr_node != NULL) {//free all nodes in queue
            temp = curr_node;
            curr_node = curr_node->next;
            free(temp);
        }
    }
    free(queue);//free queue
    queue = NULL; //setting queue to NULL 
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
    while (curr_node!=NULL) {
        printf("%d\n", curr_node->offset);
        curr_node = curr_node->next;
    }
}