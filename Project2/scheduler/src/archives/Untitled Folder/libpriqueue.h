/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

#include <stdbool.h>

#define PRIQUE_SIZE 200
/*
typedef struct _process_t {
	int pid;		       //Unique process identifier
	int arrival_time;	 //higher the arrival_time value, later it occurs in timeline
	int priority;		   //lower the priority number higher the priority
} process_t;
*/

/*
  Node Data Structure
*/
typedef struct _node_t {
	void * item;
	struct node_t * nextNode;
} node_t;

/*
  Priority Queue Data Structure
*/
typedef struct _priqueue_t {
  node_t * head;
  node_t * tail;
  int size;
  int (*comparer) (const void *, const void *);
} priqueue_t;

void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

void priqueue_print (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
