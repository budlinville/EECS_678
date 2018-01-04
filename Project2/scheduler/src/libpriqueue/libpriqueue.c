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

	if (q->size == 0) {
		q->head = newNode;
		q->tail = newNode;
		q->size++;

		return 0;
	}
	/*
	bool exists = false;
	node_t *temp = q->head;
	for (int i = 0; i < q->size; i++) {
		if (temp->item == newNode->item) {
			exists = true;
			break;
		}
		else {
			temp = temp->nextNode;
		}
	}
*/
	//if (exists == false) {



		node_t *temp = q->head;
		node_t *prev = NULL;
		int count = 0;
		while (temp != NULL) {
			if (q->scheme == SJF || q->scheme == PSJF) {


				if (q->comparer(ptr, temp->item) < 0) {


					newNode->nextNode = temp;

					//correct?
					if (prev != NULL)
						prev->nextNode = newNode;

					q->size++;

					if (count == 0) {
						q->head = newNode;
					}
					else if (count == q->size - 1) {
						q->tail = newNode;
					}

					return count;
				}
			}
			else if (q->scheme == PRI || q->scheme == PPRI) {
				if (q->comparer(ptr, temp->item) > 0) {


					newNode->nextNode = temp;

					//correct?
					if (prev != NULL)
						prev->nextNode = newNode;

					q->size++;

					if (count == 0) {
						q->head = newNode;
					}
					else if (count == q->size - 1) {
						q->tail = newNode;
					}

					return count;
				}
			}
			else {
				if (q->head != NULL) { //not empty list
					q->tail->nextNode = newNode;
					newNode->nextNode = NULL;
					q->tail = newNode;
					q->size++;

					return q->size - 1;
				}

				q->head = newNode;
				q->tail = newNode;
				q->size++;

				return q->size - 1;
			}


			if (count == 0) {
				temp = temp->nextNode;
				prev = q->head;
			} else {
				prev = temp;
				temp = temp->nextNode;

			}

			count++;
		}

		q->tail->nextNode = newNode;
		q->tail = newNode;
		q->tail->nextNode = NULL;
		q->size++;


		return(q->size - 1);
	/*}
	else {
		printf("%s\n", "Duplicate Element. Not added.\n" );
		return (q->size - 1);
	}
	*/
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
	void* temp = del->item;
	q->head = q->head->nextNode;
	q->size = q->size - 1;
	free(del);

	return temp;
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
		if (temp->nextNode != NULL){
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
				temp = q->head;
				prev = NULL;
			} else {
				del = temp;
				prev = prev;
				temp = temp->nextNode;
				count++;
			}
			numRemoved++;
		}
		if (del != NULL) {
			free(del);
			del = NULL;
		}
		else {

			if (count == 0) {
				temp = temp->nextNode;
				prev = q->head;
			} else {
				prev = temp;
				temp = temp->nextNode;

			}
			count++;
		}
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

	while (temp != NULL) {
		printf("%d ", *((int *)temp->item));
		temp = temp->nextNode;
	}
	printf("\n");
}

void setScheme(priqueue_t *q, scheme_t s) {
	q->scheme = s;
}
