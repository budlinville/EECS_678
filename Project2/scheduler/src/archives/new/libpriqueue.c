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
	//q->myArr = (void *)malloc(sizeof(int) * q->arraySize);
/*
	for (int i = 0; i < q->arraySize; i++) {
		q->myArr[i] = -1;		//flag value
		/*
		myArr[i].pid = 0;
		myArr[i].priority = 0;
		myArr[i].arrival_time = 0;
	}
	*/
	q->comparer = comparer;
	
	q->qSize = 0;
	q->front = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr) {
	int *newElem = (int *) ptr;
	q->myArr[q->qSize] = *newElem;
	q->qSize = q->qSize + 1;
	q->empty = false;

	bubbleSort(q);

	int retVal = 0;

	for (int i = 0; i < q->qSize; i++) {
		if (q->myArr[i] == *newElem) {
			retVal = i;
			break;
		}
	}

	return retVal;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q) {			//MAY NEED WORK AS FAR AS RETURNING POINTER
	if (q->empty)
		return NULL;

	int *elem = 0;
	*elem = q->myArr[0];

	return (void *) elem;
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
	if (q->empty)
		return NULL;

	int *retVal = 0;
	*retVal = q->myArr[0];


	q->myArr[0] = q->myArr[q->qSize - 1];
	q->myArr[q->qSize - 1] = 0;
	q->qSize = q->qSize - 1;

	bubbleSort(q);

	return (void *) retVal;
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
	int * FLAG = 0;
	*FLAG = -1;

	int * ret = 0;

	if (q->empty)
		return (void *)FLAG;

	if (index < 0 || index >= q->qSize)
		return (void *)FLAG;


	*ret = q->myArr[index];
	return (void *) ret;

}
/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr) {
	if (q->myArr[0] == -1) {
		return -1;
	}

	int elemToRemove = (int) ptr;
	int numRemovals = 0;
	for (int i = 0; i < q->qSize; i++) {
		if (q->myArr[i] == elemToRemove) {
			q->myArr[i] = -1;
			numRemovals++;
		}
	}
	q->qSize = q->qSize - numRemovals;

	if (q->myArr[0] == -1)
		q->empty = true;

	bubbleSort(q);


	return numRemovals;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	if (q->qSize <= index)
		return NULL;

	int *elemRemoved = 0;
	elemRemoved = q->myArr[index];

	for (int i = index; i < q->qSize; i++) {
		q->myArr[i] = q->myArr[i+1];
	}
	q->qSize = q->qSize - 1;


	if (q->myArr[0] == -1)
		q->empty = true;

	return (void *) elemRemoved;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->qSize;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	/*
	for (int i = 0; i < q->qSize; i++){
			free(q->myArr[i]);
	}
	free(q->myArr);
	*/
}

void bubbleSort(priqueue_t *q) {
   bool sorted = false; // False when swaps occur
   int pass = 1;
   while (!sorted && (pass < q->qSize)) {
      // At this point, myArr[n+1-pass..n-1] is sorted
      // and all of its entries are > the entries in myArr[0..n-pass]
      sorted = true; // Assume sorted
      for (int index = 0; index < q->qSize - pass; index++) {
         // At this point, all entries in myArr[0..index-1]
         // are <= myArr[index]
         int nextIndex = index + 1;
         if (q->myArr[index] > q->myArr[nextIndex]) {
            // Exchange entries
						int temp = q->myArr[index];
						q->myArr[index] = q->myArr[nextIndex];
						q->myArr[nextIndex] = temp;
            sorted = false; // Signal exchange
         } // end if
      }  // end for
      // Assertion: myArr[0..n-pass-1] < myArr[n-pass]

      pass++;
   }  // end while
}  // end bubbleSort
