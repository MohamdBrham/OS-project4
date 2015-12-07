
#include <stdlib.h>
#include <stdio.h>
#include "buddy.h"
#include <math.h>

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
void freeFirst(int ind){
	struct node* p;
	p = list[ind];
	list[ind ] = list[ind]->next ;  // delete the node
	p->next = list[0];              // return in after the free list ;
	list[0] = p ;
}
void freeNode(struct node * previous, struct node * p){
	if( (unsigned long) previous == (unsigned long) p  ){
		freeFirst((int)(p->size)-7);
	}else{
		previous->next = p->next ;
		p->next = list[0] ;
		list[0] = p ;
	}	
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
	base = (long long)chunkpointer;                            			 /* preserve the base pointer to the chunk */

	printf(" start base : %lu\n",(unsigned long)base);

	for(i = 0 ; i < NUM_POINTERS ; i++){            			 /* initialise the main pointers list to null */
		list[i] = NULL ;	
	}

	list[0] =  chunkpointer ;         							 /* cast the list[0] to pints to node type*/
	p = (struct node*) list[0];

	for( i = 0 ; i < pow(2 , size -8 ); i++){            		 /* connect the free nodes after list[0]*/
		 p->next =(struct node*)( ((unsigned long) p) + sizeof(struct node)) ;
		 p->size  = i ; 
		 p = p->next;
	}
	p = (struct node*)(((unsigned long) p) - sizeof(struct node)) ;
	p->next= NULL ; 
	printf(" size %d\n",size );
	 
	//list[size - 7 ]  = (struct node *)getNode();   // the main chunk stay here but we have to devide it
																										
                                                                                                         
	list[size - 8 ]  = (struct node *)getNode();                                                                       
	list[size - 8 ] ->next = NULL ;
	list[size - 8 ] ->size = size - 1 ;
	list[size - 8 ] ->ptr = (unsigned long)base + pow(2 , size - 1) ;
	list[size - 8 ] ->status = EMPTY_R;

	list[size - 9 ]  = (struct node *)getNode();
	list[size - 9 ] ->next = NULL ;
	list[size - 9 ] ->size = size - 2 ;
	list[size - 9 ] ->ptr = (unsigned long)base + pow(2 , size - 2) ;
	list[size - 9 ] ->status = EMPTY_R;

	list[size - 10 ]  = (struct node *)getNode();
	list[size - 10 ] ->next = NULL ;
	list[size - 10 ] ->size = size - 3 ;
	list[size - 10 ] ->ptr = (unsigned long)base + pow(2 , size - 3) ;
	list[size - 10 ] ->status = EMPTY_R;

	p = (struct node *)getNode();	
	p->size = size - 3 ;
	p->ptr = (unsigned long)base  ;
	p->status = FULL_L;

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
		if(p->status == EMPTY_L){
			p->status = FULL_L;
			return (void * )p->ptr; 
		}else if(p->status == EMPTY_R){
			p->status = FULL_R;
			return (void * )p->ptr; 
		}
		p = p->next ;
	}	
    int flag = 0;
	index2 = index + 1 ;
	while(index2 <= s-8 ){
		p = list[index2];
		pre = list[index2];
		while(p  != NULL){
			if(p->status == EMPTY_L || p->status == EMPTY_R  ){                     // divide the node int two nodes
				n1 = (struct node *)getNode();         
				n2= (struct node *)getNode();
				n1->size = p->size -1 ;
				n2->size = p->size -1 ;
				n1->status = EMPTY_L;
				n2->status = EMPTY_R ;
				n1->ptr = p->ptr ;
				n2->ptr = ((unsigned long)p->ptr) + (int)pow(2, n1->size) ;

				n2->next = list[index2 - 1];
				n1->next = n2 ;
				list[index2 - 1] = n1 ;
				freeNode(pre , p);
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
				pre = list[index2] ;
				while(p  != NULL){
					if(p->status == EMPTY_L || p->status == EMPTY_R  ){                     // divide the node int two nodes
						n1 = (struct node *)getNode();         
						n2= (struct node *)getNode();
						n1->size = p->size -1 ;
						n2->size = p->size -1 ;
						n1->status = EMPTY_L;
						n2->status = EMPTY_R ;
						n1->ptr = p->ptr ;
						n2->ptr = ((unsigned long)p->ptr) + (int)pow(2, n1->size) ;

						n2->next = list[index2 - 1];
						n1->next = n2 ;
						list[index2 - 1] = n1 ;
						freeNode(pre , p);
						//freeFirst(index2);
						break;		
					}
					pre =  p ;
					p = p->next ;
				}
				index2--;
			}
			p = list[index2];	// index now equal index 2 ;
			while(p  != NULL){                                /*handle the case when there is already free divided size with the desired one*/
				if(p->status == EMPTY_L){
					p->status = FULL_L;
					return (void * )p->ptr; 
				}else if(p->status == EMPTY_R){
					p->status = FULL_R;
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
	int i ,flag = 0 ,k = 0 ,count = 0;
	struct node *p , *q ,*pre1 ,*pre2;
	for( i = 1 ; i <= s- 8 ; i++){ 
		pre1 =  list[i]; 								             /* search for the desired object in all lists*/
		p = list[i];
		while(p  != NULL){
			if((unsigned long) p->ptr == (unsigned long)objectptr){
				flag = 1 ;
				k = -1 ;
				break ; /*break from while*/
			}
			pre1 = p ;
			p = p->next ;
		}		
		if( flag == 1){                           					    /*when the flag equal 1 ; that means i found the desires node */
			break ;  /*break from for*/                                 /* stop searching  , now p points to the desired node */
		}
	}
    /* in this point , if flag equal 1 , that means I found the node and
      ***  p points to the node 
      ***  pre1 point to the previos node 
      ***  i is the list number
    *  and if its still 0 , that mean the node not exist in this memory
    */

    /* first it will enter by flag == 1
      ** the next time when it have to go up to merge it will enter by k != -1 
      */
	while( flag == 1 ){

		/* now I want to search for another free node in that list*/
		//flag = 1 ;
		pre2 = list[i];
		q = list[i];                           
		while(q  != NULL){
			if( q->status == EMPTY_R && (unsigned long)q->ptr == ((unsigned long)p->ptr) + (int)pow(2,p->size)){
				flag = 2 ;
				break ;
			}else if(q->status == EMPTY_L && (unsigned long)q->ptr == ((unsigned long)p->ptr) - (int)pow(2,p->size)){
				flag = 3; 
				break;
			}
			pre2 = q ;
			q = q->next ;
		}
		/** if flag 2 or 3 
			** I found another empty node in the same list
			** pre2 point to its previous node
			** p still the same 
		*/
		if( flag == 2 ){ // I found empty node in the right
			/* k here is to know the parent its left or right*/
			k = (int)((unsigned long)p->ptr - (unsigned long) base ) / (int)pow(2,(p->size) + 1) ;		
			if( k % 2 == 0){
				freeNode(pre2 ,q ); // free the right node 
				q = (struct node *)getNode();	
				q->size = (p->size) + 1 ;
				q->ptr  = p->ptr ;   ;
				q->status = EMPTY_L;

				q->next = list[ i+1 ] ; // add the combined node to upper size	
				list[ i + 1 ] = q ;
				i += 1 ;
				if(count > 0){
					freeFirst((p->size)-7);
				}else{
					freeNode(pre1 ,p );
				}  
				pre1 = q ;
				p = q ;

			}else{ // k %2 == 1 
				freeNode(pre2 ,q ); // free the right node 
				q = (struct node *)getNode();	
				q->size = (p->size) + 1 ;
				q->ptr  = p->ptr ;   ;
				q->status = EMPTY_R;

				q->next = list[ i+1 ] ; // add the combined node to upper size	
				list[ i + 1 ] = q ;
				i += 1 ;
				if(count > 0){
					freeFirst((p->size)-7);
				}else{
					freeNode(pre1 ,p );
				} 
				pre1 = q ;
				p = q ;
			}
		}else if( flag == 3 ){// I found empty node in the left 
			k = (int)((unsigned long)q->ptr - (unsigned long) base ) / (int)pow(2,(p->size) + 1) ;		
			if( k % 2 == 0){
				freeNode(pre1 ,p ); // free the right node 
				p = (struct node *)getNode();	
				p->size = (q->size) + 1 ;
				p->ptr  = q->ptr ;   ;
				p->status = EMPTY_L;
				p->next = list[ i+1 ] ; // add the combined node to upper size	
				list[ i + 1 ] = p;
				i += 1 ;
				if(count > 0){
					freeFirst((q->size)-7);
				}else{
					freeNode(pre2 ,q );
				}				 
				pre1 = list[ i ] ;			

			}else{ // k %2 == 1 
				freeNode(pre1 ,p );  // free the right node 
				p = (struct node *)getNode();	
				p->size = (q->size) + 1 ;
				p->ptr  = q->ptr ;   ;
				p->status = EMPTY_R;
				p->next = list[ i+1 ] ; // add the combined node to upper size	
				list[ i + 1 ] = p;
				i += 1 ;
				if(count > 0){
					freeFirst((q->size)-7);
				}else{
					freeNode(pre2 ,q );
				} 
				pre1 = list[ i  ] ;
			}
		}else if(flag == 1 && k == -1){            // if  its the only free node in that list 
			if(p->status == FULL_L){
				p->status = EMPTY_L ;
			}else{
				p->status = EMPTY_R ; 
			}
			return ;
		}else if( flag == 1){
			return ;
		}
		count++;
		flag = 1 ;
	}

	return;
}

void bprint()
{
	struct node * p ;
	int i;
	for( i = 1 ; i <= s- 8 ; i++){
		p = list[i];
		while(p  != NULL){
			printf("%d %d %lld\n", p->size, p->status, p->ptr);
			p = p->next ;
		}		
	}

	//printf("the base address is %ld\n", base );
	
	//printf("bprint called\n");
	return;
}



