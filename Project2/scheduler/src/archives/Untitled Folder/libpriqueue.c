/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "libpriqueue.h"

#define PRIQUE_SIZE 200


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */

 //compare1 returns positive if a is greater; negative if b is greater
 //vice versa for compare2
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *)) {
	q->comparer = comparer;
	q->size = 0;
	q->head = NULL;
	q->tail = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */

int priqueue_offer(priqueue_t *q, void *ptr) {
	node_t *newNode = (struct node_t *)malloc( sizeof(node_t) );
	newNode->item = ptr;
	newNode->nextNode = NULL;

	if (q->head == NULL) {
		q->head = newNode;
		q->tail = newNode;
		q->size++;

		//priqueue_print(&q);

		return 0;
	}

	node_t *temp = q->head;
	node_t *prev = NULL;
	int count = 0;
	while (temp->nextNode != NULL) {
		if (q->comparer(ptr, temp->item) < 0) {
			newNode->nextNode = temp;

			//for when adding to front of list
			if (prev != NULL) {
				prev->nextNode = newNode;
			} else {
				q->head = temp;
			}

			q->size++;

			//priqueue_print(&q);

			return count;
		}

		if (count == 0) {
			temp = temp->nextNode;
			prev = q->head;
		} else {
			temp = temp->nextNode;
			prev = prev->nextNode;
		}

		count++;
	}

	q->tail->nextNode = newNode;
	q->tail = newNode;
	q->size++;


	//priqueue_print(&q);

	return(q->size - 1);
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q) {			//MAY NEED WORK AS FAR AS RETURNING POINTER
	if (q->head == NULL)
		return NULL;

	return q->head->item;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */

 //NEEDS WORK
void *priqueue_poll(priqueue_t *q) {
	if (q->head == NULL)
		return NULL;

	node_t * del = q->head;
	void* retValue = del->item;
	q->head = q->head->nextNode;
	q->size = q->size - 1;
	free(del);

	//priqueue_print(&q);

	return retValue;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index) {

	if (index > q->size || index < 0) {
		return NULL;
	}

	if (q->head == NULL){
		return NULL;
	}

	node_t *temp = q->head;

	for (int i = 0; i < index; i++) {
		//printf("%d: %d\n", i, temp->item);
		if (temp->nextNode != NULL){
			//printf("\nVALID");
			temp = temp->nextNode;
		}
	}

	//if (temp->item == NULL)
		//return NULL;

	return temp->item;
}
/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
 /*
int priqueue_remove(priqueue_t *q, void *ptr) {
	if (q->head == NULL)
		return 0;

	int numRemoved = 0;
	int count = 0;
	node_t *temp = q->head;
	node_t *prev = NULL;
	node_t *del = NULL;

	while (temp != NULL) {
		if (temp->item == ptr) {
			if (count == 0) {
				del = temp;
				q->head = temp->nextNode;
			} else {
				del = temp;
				prev->nextNode = temp->nextNode;
			}
			numRemoved++;
		}

		//increment
		if (count == 0) {
			temp = temp->nextNode;
			prev = q->head;
		} else {
			temp = temp->nextNode;
			prev = prev->nextNode;
		}

		if(del != NULL) {
			free(del);
			del = NULL;
		}
		count++;
	}

	q->size = q->size - numRemoved;

	//priqueue_print(&q);


	return numRemoved;
}
*/
int priqueue_remove(priqueue_t *q, void *ptr) {
	if (q->head == NULL)
		return 0;

	int numRemoved = 0;
	int count = 0;
	node_t *temp = q->head;
	node_t *next = NULL;
	node_t *del = NULL;

	while (temp->nextNode != NULL) {
		next = temp->nextNode;
		if ((next->item) == ptr) {
			del = temp->nextNode;
			temp->nextNode = next->nextNode;

			free(del);
			numRemoved++;
		}
		temp = temp->nextNode;
	}
	q->size = q->size - numRemoved;

	return numRemoved;

}

/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */

 //may need work
void *priqueue_remove_at(priqueue_t *q, int index) {
	if (q->head == NULL)
		return NULL;

	if (index > q->size || index < 0)
		return NULL;

	if (index == 0) {
		node_t *temp = q->head;
		q->head = temp->nextNode;
		free(temp);
	}

	int count = 0;
	node_t *temp = q->head;
	node_t *prev = NULL;
	while (temp != NULL) {
		if (count == index) {
			prev->nextNode = temp->nextNode;
			free(temp);
			q->size--;
		}



		if (count == 0) {
			temp = temp->nextNode;
			prev = q->head;
		} else {
			temp = temp->nextNode;
			prev = prev->nextNode;
		}
		count++;
	}

	return NULL;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q) {
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q) {

}

void priqueue_print (priqueue_t *q) {
	node_t *temp = q->head;

	printf("\n");
	while (temp != NULL) {
		printf("%d ", *((int *)temp->item));
		temp = temp->nextNode;
	}
	printf("\n");
}
