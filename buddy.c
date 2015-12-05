
#include <stdlib.h>
#include <stdio.h>
#include "buddy.h"
#include <math.h>

// printfs are for debugging; remove them when you use/submit your library
long long base;

struct node
{
    char size;                       /* size of the pointed chunck */
	char status;
    long long  ptr;			/* pointer to the chunk */  
	struct node* next ;   
};

struct node* list[NUM_POINTERS];


int roundSize(int size){	
	return (int)ceil(log(size) / log(2));
}
void freeNode(struct node * n){
	n->next = list[0] ;
	list[0] = n ;
	n = NULL ;
}
struct node* getNode(){
	struct node * p ;
	p = list[0];
	list[0] = list[0]->next;
	p->next = NULL ;
	 
	return p ;
}

int binit(void *chunkpointer, int chunksize)
{	
	int i ; 
	struct node* p;
	char size = (char)roundSize(chunksize) ;
	//printf(" size %d\n",size );
	char tsize ;
	base = chunkpointer;

	for(i = 0 ; i < NUM_POINTERS ; i++){            /* initialise the main pointers list to null */
		list[i] = NULL ;	
	}

	list[0] = (struct node*) chunkpointer ;         /* cast the list[0] to pints to node type*/
	p = list[0];
	

	for( i = 0 ; i < pow(2 , size -8 )  ; i++){     /* connect the free nodes after list[0]*/
		p->next = p + 18 ;
		p->size  = i ; 
		p = p->next ; 
	}
	printf(" size %d\n",size );
	p = NULL ; 
	printf(" size %d\n",size );
	 
	//list[size - 7 ]  = (struct node *)getNode();   // the main chunk stay here but we have to devide it

	

	list[size - 8 ]  = (struct node *)getNode();
	list[size - 8 ] ->next = NULL ;
	list[size - 8 ] ->size = size - 1 ;
	list[size - 8 ] ->ptr = base + pow(2 , size - 1) ;
	list[size - 8 ] ->status = EMPTY;

	list[size - 9 ]  = (struct node *)getNode();
	list[size - 9 ] ->next = NULL ;
	list[size - 9 ] ->size = size - 2 ;
	list[size - 9 ] ->ptr = base + pow(2 , size - 2) ;
	list[size - 9 ] ->status = EMPTY;

	list[size - 10 ]  = (struct node *)getNode();
	list[size - 10 ] ->next = NULL ;
	list[size - 10 ] ->size = size - 3;
	list[size - 10 ] ->ptr = base + pow(2 , size - 3) ;
	list[size - 10 ] ->status = EMPTY;

	list[size - 11 ]  = (struct node *)getNode();
	list[size - 11 ] ->next = NULL ;
	list[size - 11 ] ->size = size - 4 ;
	list[size - 11 ] ->ptr = base + pow(2 , size - 4) ;
	list[size - 11 ] ->status = EMPTY;

	p = (struct node *)getNode();	
	p->size = size - 4 ;
	p->ptr = base  ;
	p->status = FULL;

	p->next = list[size - 11 ] ; 
	list[size - 11 ] = p ;

	
	/*for( i = size - 7 ; i > size - 7 - 4 ; i--){

	}*/

	printf("the base address is %ld\n", base );
	p = list[size - 8];
	while(p  != NULL){
		printf("%d %d %ld\n", p->size, p->status, p->ptr);
		p = p->next ;
	}
	p = list[size - 9];
	while(p  != NULL){
		printf("%d %d %ld\n", p->size, p->status, p->ptr);
		p = p->next ;
	}
	p = list[size - 10];
	while(p  != NULL){
		printf("%d %d %ld\n", p->size, p->status, p->ptr);
		p = p->next ;
	}
	 p = list[size - 11];
	while(p  != NULL){
		printf("%d %d %ld\n", p->size, p->status, p->ptr);
		p = p->next ;
	}

	//list[roundSize(chunksize) - 7 ] = chunkpointer ; 
	
	

	return (0);		// if success
	
}

void *balloc(int objectsize)
{


	printf("balloc called\n");

	return (NULL);		// if not success
}

void bfree(void *objectptr)
{

	printf("bfree called\n");

	return;
}

void bprint(void)
{
	printf("bprint called\n");
	return;
}



