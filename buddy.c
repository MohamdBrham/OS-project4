
#include <stdlib.h>
#include <stdio.h>
#include "buddy.h"
#include <math.h>
#define SIZE_OF_NODE 18

// printfs are for debugging; remove them when you use/submit your library
long long base;
int s ;
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
void freeNode(struct node * previous, struct node * p){
	if( (unsigned long) previous == (unsigned long) p  ){
		previous = previous->next ; 
		/*n->next = list[0] ;
		list[0] = n ;*/
	}else{
		previous->next = p->next ;
		p->next = list[0] ;
		list[0] = p ;
	}	
}
void freeFirst(int ind){
	struct node* p;
	p = list[ind];
	list[ind ] = list[ind]->next ;  // delete the node
	p->next = list[0];              // return in after the free list ;
	list[0] = p ;

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
	char size = (char)roundSize(chunksize) ;         			/* here i deal with the size just by the power*/
	if(size > 15 || size < 5){                       			/*the minimum size is 32 KB and the max size is 32 MB */
		return -1 ;
	}
	size  = size + 10 ;	 
	s = size ;                           			 /* add 1024 to the size becaouse its already entered in KB*/
	base = chunkpointer;                            			 /* preserve the base pointer to the chunk */

	printf(" start base : %lu\n",(unsigned long)base);

	for(i = 0 ; i < NUM_POINTERS ; i++){            			 /* initialise the main pointers list to null */
		list[i] = NULL ;	
	}

	list[0] =  chunkpointer ;         							 /* cast the list[0] to pints to node type*/
	p = (struct node*) list[0];
		printf(" size is %d\n",sizeof(struct node) );

	for( i = 0 ; i < pow(2 , size -8 ); i++){            		 /* connect the free nodes after list[0]*/
		 p->next = ((unsigned long) p) + sizeof(struct node) ;
		 p->size  = i ; 
		 p = p->next;
	}
	p = ((unsigned long) p) - sizeof(struct node) ;
	p->next= NULL ; 
	printf(" size %d\n",size );
	 
	//list[size - 7 ]  = (struct node *)getNode();   // the main chunk stay here but we have to devide it
																										
                                                                                                         
	list[size - 8 ]  = (struct node *)getNode();                                                                       
	list[size - 8 ] ->next = NULL ;
	list[size - 8 ] ->size = size - 1 ;
	list[size - 8 ] ->ptr = (unsigned long)base + pow(2 , size - 1) ;
	list[size - 8 ] ->status = EMPTY;

	list[size - 9 ]  = (struct node *)getNode();
	list[size - 9 ] ->next = NULL ;
	list[size - 9 ] ->size = size - 2 ;
	list[size - 9 ] ->ptr = (unsigned long)base + pow(2 , size - 2) ;
	list[size - 9 ] ->status = EMPTY;

	list[size - 10 ]  = (struct node *)getNode();
	list[size - 10 ] ->next = NULL ;
	list[size - 10 ] ->size = size - 3 ;
	list[size - 10 ] ->ptr = (unsigned long)base + pow(2 , size - 3) ;
	list[size - 10 ] ->status = EMPTY;

	p = (struct node *)getNode();	
	p->size = size - 3 ;
	p->ptr = (unsigned long)base  ;
	p->status = FULL;

	p->next = list[size - 10 ] ; 
	list[size - 10 ] = p ;

	
	/*for( i = size - 7 ; i > size - 7 - 4 ; i--){

	}*/

	return (0);		// if success
	
}

void *balloc(int objectsize)
{
	struct node *p ,*n1 ,*n2 ,*pre;
	int size = roundSize(objectsize);
	int index2 , index = size - 7 ;                     		/*the list with index size-7 will express this size*/
	p = list[index];	
	while(p  != NULL){                                /*handle the case when there is already free divided size with the desired one*/
		if(p->status == EMPTY){
			p->status = FULL;
			printf("bloch with size : %d is alocated\n",(int)pow(2,size));
			return (void * )p->ptr; 
		}
		p = p->next ;
	}	
    int flag = 0;
    int count = 1;
	index2 = index + 1 ;
	while(index2 <= s-8 ){
		p = list[index2];
		pre = list[index2];
		while(p  != NULL){
			if(p->status == EMPTY){                     // divide the node int two nodes
				n1 = (struct node *)getNode();         
				n2= (struct node *)getNode();
				n1->size = p->size -1 ;
				n2->size = p->size -1 ;
				n1->status = EMPTY;
				n2->status = EMPTY ;
				n1->ptr = p->ptr ;
				n2->ptr = ((unsigned long)p->ptr) + (int)pow(2, n1->size) ;

				n2->next = list[index2 - 1];
				n1->next = n2 ;
				list[index2 - 1] = n1 ;
				printf("free is called at size first %d\n", p->size);
				freeNode(pre , p);
				bprint();				
				flag = 1;
				break ;
			}
			pre = p ;
			p = p->next ;
		}
		if( flag == 1){
			index2--;
			while(index2 > index){
				p = list[index2];
				while(p  != NULL){
					if(p->status == EMPTY){                     // divide the node int two nodes
						n1 = (struct node *)getNode();         
						n2= (struct node *)getNode();
						n1->size = p->size -1 ;
						n2->size = p->size -1 ;
						n1->status = EMPTY;
						n2->status = EMPTY ;
						n1->ptr = p->ptr ;
						n2->ptr = ((unsigned long)p->ptr) + (int)pow(2, n1->size) ;

						n2->next = list[index2 - 1];
						n1->next = n2 ;
						list[index2 - 1] = n1 ;
						printf("free is called at size second %d\n", p->size);
						bprint();
						freeFirst(index2);
						printf("after\n");
						bprint();
						break;		
					}
					p = p->next ;
				}
				index2--;
			}
			p = list[index2];	// index now equal index 2 ;
			while(p  != NULL){                                /*handle the case when there is already free divided size with the desired one*/
				if(p->status == EMPTY){
					p->status = FULL;
					printf("block with size : %d is alocated after division \n",(int)pow(2,size));
					return (void * )p->ptr; 
				}
				p = p->next ;
			}	
		}
		index2++;
	}

	printf("balloc called\n");

	return (NULL);		// if not success
}

void bfree(void *objectptr)
{
	int i ,flag = 0;

	for( i = 1 ; i <= s- 8 ; i++){  // search for the desired object
		p = list[i];
		while(p  != NULL){
			if((unsigned long) p->ptr == (unsigned long)objectptr){
				flag = 1 ;
				break ;
			}
			p = p->next ;
		}		
		if( flag == 1){
			break ;
		}
	}
	if(flag == 1){
		p = list[i];
		while(p  != NULL){
			if(p->status == EMPTY && abs(p->status )){
				flag = 1 ;
				break ;
			}
			p = p->next ;
		}
	}


	printf("bfree called\n");

	return;
}

void bprint()
{
	struct node * p ;
	int i;
	for( i = 1 ; i <= s- 8 ; i++){
		p = list[i];
		while(p  != NULL){
			printf("%d %d %ld\n", p->size, p->status, p->ptr);
			p = p->next ;
		}		
	}

	//printf("the base address is %ld\n", base );
	
	//printf("bprint called\n");
	return;
}



