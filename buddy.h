#ifndef BUDDY_H
#define BUDDY_H
#define NUM_POINTERS 19
#define FULL 1
#define EMPTY 0

int binit(void *, int);
void *balloc(int);
void bfree(void *);
void bprint(void); 
int roundSize(int );

#endif
