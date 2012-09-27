/**
 * CS 2110 - Fall 2011 - Homework #11
 * Created by: Brandon Whitehead
 * Edited by: Tanner Smith
 *
 * list.c: Complete the functions!
 **/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"

/* The node struct.  Has a prev pointer, next pointer, and data. */
/* DO NOT DEFINE ANYTHING OTHER THAN WHAT'S GIVEN OR YOU WILL GET A ZERO*/
/* Design consideration only this file should know about nodes */
/* Only this file should be manipulating nodes */
typedef struct lnode
{
	struct lnode* prev; /* Pointer to previous node */
	struct lnode* next; /* Pointer to next node */
	void* data; /* User data */
} node;


/* Do not create any global variables here. Your linked list library should obviously work for multiple linked lists */
/* This function is declared as static since you should only be calling this inside this file. */
static node* create_node(void* data);

/** create_list
	*
	* Creates a list by allocating memory for it on the heap.
	* Be sure to initialize size to zero and head to NULL.
	*
	* @return an empty linked list
	*/
list* create_list(void)
{
	list *newList = malloc(sizeof(list));
	assert(newList);

	newList->head = NULL;
	newList->size = 0;

	return newList;
}

/** create_node
	*
	* Helper function that creates a node by allocating memory for it on the heap.
	* Be sure to set its pointers to NULL.
	*
	* @param data a void pointer to data the user wants to store in the list
	* @return a node
	*/
static node* create_node(void* data)
{
	node *newNode = malloc(sizeof(node));
	assert(newNode);

	newNode->prev = NULL;
	newNode->next = NULL;
	newNode->data = data;

	return newNode;
}

/** push_front
	*
	* Adds the data to the front of the linked list
	*
	* @param llist a pointer to the list.
	* @param data pointer to data the user wants to store in the list.
	*/
void push_front(list* llist, void* data)
{
	node *newNode = create_node(data);

	if (llist->head == NULL) {
		llist->head = newNode;

		newNode->next = newNode;
		newNode->prev = newNode;

		llist->size = 1;
	} else {
		node *tail = llist->head->prev;

		newNode->prev = tail;
		tail->next = newNode;

		newNode->next = llist->head;
		llist->head->prev = newNode;

		llist->head = newNode;
		llist->size++;
	}
}

/** push_back
	*
	* Adds the data to the back/end of the linked list
	*
	* @param llist a pointer to the list.
	* @param data pointer to data the user wants to store in the list.
	*/
void push_back(list* llist, void* data)
{
	node *newNode = create_node(data);

	if (llist->head == NULL) {
		llist->head = newNode;

		newNode->next = newNode;
		newNode->prev = newNode;

		llist->size = 1;
	} else {
		node *tail = llist->head->prev;

		newNode->prev = tail;
		tail->next = newNode;

		newNode->next = llist->head;
		llist->head->prev = newNode;

		llist->size++;
	}
}

/** remove_front
	*
	* Removes the node at the front of the linked list
	*
	* @warning Note the data the node is pointing to is also freed. If you have any pointers to this node's data it will be freed!
	*
	* @param llist a pointer to the list.
	* @param free_func pointer to a function that is responsible for freeing the node's data.
	* @return -1 if the remove failed (which is only there are no elements) 0 if the remove succeeded.
	*/
int remove_front(list* llist, list_op free_func)
{
	if (llist->head != NULL) {
		if (llist->size == 1) {
			/* If the list contains only 1 element... */
			free_func(llist->head->data);
			free(llist->head);

			llist->head = NULL;
			llist->size = 0;

			return 0;
		} else {
			/* If the list has 1+ elements... */
			node *tail = llist->head->prev;
			node *headNext = llist->head->next;

			free_func(llist->head->data);
			free(llist->head);

			tail->next = headNext;
			headNext->prev = tail;

			llist->head = headNext;
			llist->size--;

			return 0;
		}
	}

	return -1;
}

/** remove_index
	*
	* Removes the indexth node of the linked list
	*
	* @warning Note the data the node is pointing to is also freed. If you have any pointers to this node's data it will be freed!
	*
	* @param llist a pointer to the list.
	* @param index index of the node to remove.
	* @param free_func pointer to a function that is responsible for freeing the node's data.
	* @return -1 if the remove failed 0 if the remove succeeded.
	*/
int remove_index(list* llist, int index, list_op free_func)
{
	node *temp, *prev, *next;
	int currentIndex = 0;

	if (index < 0 || index > size(llist) - 1) {
		return -1;
	}

	temp = llist->head;

	do {
		if (currentIndex == index) {
			prev = temp->prev;
			next = temp->next;

			prev->next = next;
			next->prev = prev;

			if (currentIndex == 0) {
				if (next != llist->head) {
					llist->head = next;
				} else {
					llist->head = NULL;
				}
			}

			free_func(temp->data);
			free(temp);

			llist->size--;

			return 0;
		}

		temp = temp->next;
		currentIndex++;
	} while (temp != llist->head);

	return -1;
}

/** remove_back
	*
	* Removes the node at the back of the linked list
	*
	* @warning Note the data the node is pointing to is also freed. If you have any pointers to this node's data it will be freed!
	*
	* @param llist a pointer to the list.
	* @param free_func pointer to a function that is responsible for freeing the node's data.
	* @return -1 if the remove failed 0 if the remove succeeded.
	*/
int remove_back(list* llist, list_op free_func)
{
	if (llist->head != NULL) {
		if (llist->size == 1) {
			node *tail = llist->head->prev;

			free_func(tail->data);
			free(tail);

			llist->head = NULL;
			llist->size --;

			return 0;
		} else if (llist->size > 1) {
			node *tail = llist->head->prev;

			node *prev = tail->prev;

			prev->next = llist->head;
			llist->head->prev = prev;

			free_func(tail->data);
			free(tail);

			llist->size--;

			return 0;
		}
	}

	return -1;
}

/** remove_data
	*
	* Removes ALL nodes whose data is EQUAL to the data you passed in or rather when the comparison function returns true (!0)
	* @warning Note the data the node is pointing to is also freed. If you have any pointers to this node's data it will be freed!
	*
	* @param llist a pointer to the list
	* @param data data to compare to.
	* @param compare_func a pointer to a function that when it returns true it will remove the element from the list and do nothing otherwise @see equal_op.
	* @param free_func a pointer to a function that is responsible for freeing the node's data
	* @return the number of nodes that were removed.
	*/
int remove_data(list* llist, const void* data, equal_op compare_func, list_op free_func)
{
	node *temp = llist->head;
	int count = 0;
	int index = 0;

	int done = 0;

	if (temp != NULL) {
		while (!done) {
			node *next = temp->next;

			if (compare_func(data, temp->data)) {
				remove_index(llist, index, free_func);

				count++;
			} else {
				index++;
			}

			if (index == size(llist)) {
				done = 1;
			}

			temp = next;
		}
	}

	return count;
}

/** remove_if
	*
	* Removes all nodes whose data when passed into the predicate function returns true
	*
	* @param llist a pointer to the list
	* @param pred_func a pointer to a function that when it returns true it will remove the element from the list and do nothing otherwise @see list_pred.
	* @param free_func a pointer to a function that is responsible for freeing the node's data
	* @return the number of nodes that were removed.
	*/
int remove_if(list* llist, list_pred pred_func, list_op free_func)
{
	node *temp = llist->head;
	node *next;
	int count = 0;
	int index = 0;

	int done = 0;

	if (temp != NULL) {
		while (!done) {
			next = temp->next;

			if (pred_func(temp->data)) {
				remove_index(llist, index, free_func);

				count++;
			} else {
				index++;
			}

			if (index == size(llist)) {
				done = 1;
			}

			temp = next;
		}
	}

	return count;
}

/** front
	*
	* Gets the data at the front of the linked list
	* If the list is empty return NULL.
	*
	* @param llist a pointer to the list
	* @return The data at the first node in the linked list or NULL.
	*/
void* front(list* llist)
{
	if (llist->head != NULL) {
		return llist->head->data;
	}

	return NULL;
}

/** get_index
	*
	* Gets the data at the indexth node of the linked list
	* If the list is empty or if the index is invalid return NULL.
	*
	* @param llist a pointer to the list
	* @return The data at the indexth node in the linked list or NULL.
	*/
void* get_index(list* llist, int index)
{
	node *temp = llist->head;
	int currentIndex = 0;

	if (index < 0 || index > size(llist) - 1) {
		return NULL;
	}

	do {
		if (currentIndex == index) {
			return temp->data;
		}

		temp = temp->next;
		currentIndex++;
	} while (temp != llist->head);

	return NULL;
}

/** back
	*
	* Gets the data at the "end" of the linked list
	* If the list is empty return NULL.
	*
	* @param llist a pointer to the list
	* @return The data at the last node in the linked list or NULL.
	*/
void* back(list* llist)
{
	if (llist->head == NULL && llist->size == 0) {
		return NULL;
	}

	return llist->head->prev->data;
}

/** is_empty
	*
	* Checks to see if the list is empty.
	*
	* @param llist a pointer to the list
	* @return 1 if the list is indeed empty 0 otherwise.
	*/
int is_empty(list* llist)
{
	return llist->size == 0 && llist->head == NULL;
}

/** size
	*
	* Gets the size of the linked list
	*
	* @param llist a pointer to the list
	* @return The size of the linked list
	*/
int size(list* llist)
{
	return llist->size;
}

/** find_occurence
	*
	* Tests if the search data passed in is in the linked list.
	*
	* @param llist a pointer to a linked list.
	* @param search data to search for the occurence.
	* @param compare_func a pointer to a function that returns true if two data items are equal @see equal_op.
	* @return The data if the data is indeed in the linked list 0 otherwise.
	*/
void * find_occurrence(list* llist, const void* search, equal_op compare_func)
{
	node *temp = llist->head;

	if (temp != NULL) {
		do {
			if (compare_func(search, temp->data)) {
				return temp->data;
			}
			temp = temp->next;
		} while (temp != llist->head);
	}

	return 0;
}

/** empty_list
	*
	* Empties the list after this is called the list should be empty.
	*
	* @param llist a pointer to a linked list.
	* @param free_func function used to free the node's data.
	*
	*/
void empty_list(list* llist, list_op free_func)
{
	node *temp;

	traverse(llist, free_func);

	temp = llist->head;

	if (temp != NULL) {
		do {
			node *next = temp->next;
			free(temp);
			temp = next;
		} while (temp != llist->head);
	}

	llist->head = NULL;
	llist->size = 0;
}

/** traverse
	*
	* Traverses the linked list calling a function on each node's data.
	*
	* @param llist a pointer to a linked list.
	* @param do_func a function that does something to each node's data.
	*/
void traverse(list* llist, list_op do_func)
{
	node *temp = llist->head;

	if (temp != NULL) {
		do {
			do_func(temp->data);
			temp = temp->next;
		} while (temp != llist->head);
	}
}
