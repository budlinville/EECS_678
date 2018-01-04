#ifndef BUDDY_H
#define BUDDY_H

void buddy_init();
void *buddy_alloc(int size);
void buddy_free(void *addr);
void buddy_dump();

int getSmallestOrder();
void split (int index, int currentOrder, int finalOrder);

#endif // BUDDY_H
