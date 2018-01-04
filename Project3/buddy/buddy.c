/**
* Buddy Allocator
*
* For the list library usage, see http://www.mcs.anl.gov/~kazutomo/list/
*/

/**************************************************************************
* Conditional Compilation Options
**************************************************************************/
#define USE_DEBUG 0

/**************************************************************************
* Included Files
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "buddy.h"
#include "list.h"
#include <math.h>

/**************************************************************************
* Public Definitions
**************************************************************************/
#define MIN_ORDER 12
#define MAX_ORDER 20

#define PAGE_SIZE (1<<MIN_ORDER)
/* page index to address */
#define PAGE_TO_ADDR(page_idx) (void *)((page_idx*PAGE_SIZE) + g_memory)

/* address to page index */
#define ADDR_TO_PAGE(addr) ((unsigned long)((void *)addr - (void *)g_memory) / PAGE_SIZE)

/* find buddy address */
#define BUDDY_ADDR(addr, o) (void *)((((unsigned long)addr - (unsigned long)g_memory) ^ (1<<o)) \
									+ (unsigned long)g_memory)

#if USE_DEBUG == 1
#  define PDEBUG(fmt, ...) \
 fprintf(stderr, "%s(), %s:%d: " fmt,			\
	 __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#  define IFDEBUG(x) x
#else
#  define PDEBUG(fmt, ...)
#  define IFDEBUG(x)
#endif



/**************************************************************************
* Public Types
**************************************************************************/
typedef struct {
 struct list_head list;
 /* TODO: DECLARE NECESSARY MEMBER VARIABLES */
 int order;
 int index;
 char* memPtr;
} page_t;

/**************************************************************************
* Global Variables
**************************************************************************/
/* free lists*/
struct list_head free_area[MAX_ORDER+1];

/* memory area */
char g_memory[1<<MAX_ORDER];

/* page structures */
page_t g_pages[(1<<MAX_ORDER)/PAGE_SIZE];

/**************************************************************************
* Public Function Prototypes
**************************************************************************/

/**************************************************************************
* Local Functions
**************************************************************************/

int getSmallestOrder(int size) {
	//printf("GETTING ORDER - SIZE IS %d\n", size);
	int order = 0;
	int x = 1;
	while (x < size) {
		x = x*2;
		order++;
		//printf("GSO (order: %d) (size: %d)\n", order, x);
	}
	return order;
}

void divide(int index, int currentOrder, int finalOrder) {
	//printf("RECURSING; curOrder: %d\n", currentOrder);
	if (currentOrder == finalOrder) {
		return;
	}



	page_t* rSide = &g_pages[ADDR_TO_PAGE(BUDDY_ADDR(PAGE_TO_ADDR(index), (currentOrder - 1)))];

	//always add right side of split to free area
	list_add(&(rSide->list), &free_area[currentOrder - 1]);

	divide(index, currentOrder - 1, finalOrder);
}

/**
* Initialize the buddy system
*/
void buddy_init()
{
 int i;
 int n_pages = (1<<MAX_ORDER) / PAGE_SIZE;
 for (i = 0; i < n_pages; i++) {
	 /* TODO: INITIALIZE PAGE STRUCTURES */

	 INIT_LIST_HEAD(&g_pages[i].list);
	 g_pages[i].memPtr = PAGE_TO_ADDR(i);
	 g_pages[i].order = -1;
	 g_pages[i].index = i;

 }

 g_pages[0].order = MAX_ORDER;

 /* initialize freelist */
 for (i = MIN_ORDER; i <= MAX_ORDER; i++) {
	 INIT_LIST_HEAD(&free_area[i]);
 }

 /* add the entire memory as a freeblock */
 list_add(&g_pages[0].list, &free_area[MAX_ORDER]);
}

/**
* Allocate a memory block.
*
* On a memory request, the allocator returns the head of a free-list of the
* matching size (i.e., smallest block that satisfies the request). If the
* free-list of the matching block size is empty, then a larger block size will
* be selected. The selected (large) block is then splitted into two smaller
* blocks. Among the two blocks, left block will be used for allocation or be
* further splitted while the right block will be added to the appropriate
* free-list.
*
* @param size size in bytes
* @return memory block address
*/
void *buddy_alloc(int size)
{
	int memBlock = 1 << MAX_ORDER;

	if (size <= 0 || size > memBlock) {
 		//printf("ERROR: INVALID SIZE");
 		return NULL;
 	}

  //get Order of smallest block size
  //int finalOrder = getSmallestOrder(size);
  //printf("FINALORDER: %d\n", finalOrder);
  int orderIndex = MIN_ORDER;

	memBlock = 1 << orderIndex;

	orderIndex = getSmallestOrder(size);

/*
  if (finalOrder > MAX_ORDER) {
 	 printf("ERROR - size of data is too large\n");
 	 return NULL;
  }
	*/
 	/*
  //check to see if a block of memory of size (2 ^ order) is available
  //if not, I need to go up one
  while (list_empty(&free_area[orderIndex]) && orderIndex <= MAX_ORDER) {
 	 printf("LOOPING: free_area at order %d is not available\n", orderIndex);
 	 orderIndex++;
  }
  printf("Order %d chosen.\n", orderIndex);

  page_t* firstFreePage = list_entry(free_area[orderIndex].next, page_t, list);
  int index = firstFreePage->buddyIndex;
  list_del_init(&(firstFreePage->list));

  divide(index, orderIndex, finalOrder);

  return ((void*)PAGE_TO_ADDR(index));
  */
 	int i;
  	for (i = orderIndex; i <= MAX_ORDER; i++) {

		if (list_empty(&free_area[i])) {
			//can add to this block

			
		} else {

			int reqIndex;
 			void* reqAddr;
 			page_t *leftBlock;
 			page_t *rightBlock;


 			if (i == orderIndex) {
 				//get leftside and delete entry
 				leftBlock = list_entry(free_area[i].next, page_t, list);
 				list_del(&(leftBlock->list));
 			} else {

				memBlock = 1 << (orderIndex + 1);
 				leftBlock = &g_pages[ADDR_TO_PAGE(buddy_alloc(memBlock))];


				memBlock = 1 << orderIndex;
 				reqIndex = leftBlock->index + memBlock / PAGE_SIZE;
 				rightBlock = &g_pages[reqIndex];

 				list_add(&(rightBlock->list), &free_area[orderIndex]);
 			}

 			leftBlock->order = orderIndex;

 			reqAddr = PAGE_TO_ADDR(leftBlock->index);
 			return reqAddr;
 		}
  	}

 	return NULL;
}

/**
* Free an allocated memory block.
*
* Whenever a block is freed, the allocator checks its buddy. If the buddy is
* free as well, then the two buddies are combined to form a bigger block. This
* process continues until one of the buddies is not free.
*
* @param addr memory block address to be freed
*/
void buddy_free(void *addr)
{
 /* TODO: IMPLEMENT THIS FUNCTION */

 //Get address of the buddy
 int pageIndex = ADDR_TO_PAGE(addr);
 int pageOrder = g_pages[pageIndex].order;
 page_t* pageToDelete;
 struct list_head* l;





 //Buddy is free

 //Merge the two blocks repeatedly until no more merging is possible
 while (1) {

	 pageToDelete = NULL;


	 //Search for buddy
	 list_for_each(l, &free_area[pageOrder]) {

		 pageToDelete = list_entry(l, page_t, list);
		 if (pageToDelete == NULL || pageToDelete->memPtr == BUDDY_ADDR(addr, pageOrder)) {
			 break;
		 }



	 }

	 //No free blocks of right size
	 if (pageToDelete == NULL || pageToDelete->memPtr != BUDDY_ADDR(addr, pageOrder)) {
			g_pages[pageIndex].order = -1;
			list_add(&g_pages[pageIndex].list, &free_area[pageOrder]);
			return;
	 }



	 //Found buddy

	 if ((char *)addr > pageToDelete->memPtr) {
		 addr = pageToDelete->memPtr;

		 pageIndex = ADDR_TO_PAGE(addr);
	 }

	 list_del(&(pageToDelete->list));

	 pageOrder++;


 }


}

/**
* Print the buddy system status---order oriented
*
* print free pages in each order.
*/
void buddy_dump()
{
 int o;
 for (o = MIN_ORDER; o <= MAX_ORDER; o++) {
	 struct list_head *pos;
	 int cnt = 0;
	 list_for_each(pos, &free_area[o]) {
		 cnt++;
	 }
	 printf("%d:%dK ", cnt, (1<<o)/1024);
 }
 printf("\n");
}
