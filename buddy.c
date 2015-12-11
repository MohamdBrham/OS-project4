
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
    int  ptr;			/* pointer to the chunk */  
	struct node* next ;   
};

struct node* list[NUM_POINTERS];


int roundSize(int size){
	int power = 1 ,count = 0;
	while(power < size){
 	   power = power << 1;
 	   count++;
	}
	return count ;
}
int powOfTwo(int power){
	return(1 << power);
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
int binit(void *chunkpointer, int chunksize){	
	int i ; 
	struct node* p;
	char size = (char)roundSize(chunksize) ;         			/* here i deal with the size just by the power*/
	if(size > 15 || size < 5){                       			/*the minimum size is 32 KB and the max size is 32 MB */
		return -1 ;
	}
	size  = size + 10 ;	 
	s = size ;      
	                     			 /* add 1024 to the size becaouse its already entered in KB*/
	base = (long long)chunkpointer;                            			 /* preserve the base pointer to the chunk */
	for(i = 0 ; i < NUM_POINTERS ; i++){            			 /* initialise the main pointers list to null */
		list[i] = NULL ;	
	}
	list[0] =  chunkpointer ;         							 /* cast the list[0] to pints to node type*/
	p = (struct node*) list[0];

	for( i = 0 ; i < powOfTwo(size -8); i++){            		 /* connect the free nodes after list[0]*/
		 p->next =(struct node*)( ((unsigned long) p) + sizeof(struct node)) ;		  
		 p = p->next;
	}

	p = (struct node*)(((unsigned long) p) - sizeof(struct node)) ;
	p->next= NULL ; 	
	 
	//list[size - 7 ]  = (struct node *)getNode();   // the main chunk stay here but we have to devide it
																										
    for( i = 1 ; i < 5 ; i++ ){
    	list[size - 7-i ]  = (struct node *)getNode();                                                                       
		list[size - 7-i ] ->next = NULL ;
		list[size - 7-i ] ->size = size - i ;		
		list[size - 7-i ] ->ptr =  (int) powOfTwo(size -i) ;
		list[size - 7-i ] ->status = EMPTY_R;
	}
	 // allocate 1/16 of the main memory for data managment
	// first allocate 2/32 of the main chunk
	p = (struct node *)getNode();          
	p->size = size - 4 ;
	p->ptr = 0  ;
	p->status = FULL_L;                        

	p->next = list[size - 11 ] ; 
	list[size - 11 ] = p ;
	

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
			return (void * )(p->ptr+(long long ) base); 
		}else if(p->status == EMPTY_R){
			p->status = FULL_R;
			return (void * )(p->ptr+(long long ) base); 
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
				n2 = (struct node *)getNode();
				n1->size = p->size -1 ;
				n2->size = p->size -1 ;
				n1->status = EMPTY_L;
				n2->status = EMPTY_R ;
				n1->ptr = p->ptr ;
				n2->ptr = p->ptr + powOfTwo(n1->size) ;

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
						n2->ptr = ((int)p->ptr) + (int)powOfTwo(n1->size) ;

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
					return (void * )(p->ptr +(long long ) base); 
				}else if(p->status == EMPTY_R){
					p->status = FULL_R;
					return (void * )(p->ptr+(long long ) base); 
				}
				p = p->next ;
			}	
		}
		index2++;
	}
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
			if((int) p->ptr == ((long long)objectptr) - (long long)base){
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
			if( q->status == EMPTY_R && (int)q->ptr == ((int)p->ptr) + (int)powOfTwo(p->size)){
				flag = 2 ;
				break ;
			}else if(q->status == EMPTY_L && (int)q->ptr == ((int)p->ptr) - (int)powOfTwo(p->size)){
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
			k =  p->ptr / (powOfTwo(p->size + 1 ) ) ;	
			freeNode(pre2 ,q ); // free the right node 
			q = (struct node *)getNode();	
			q->size = (p->size) + 1 ;
			q->ptr  = p->ptr ;
			if( k % 2 == 0){				
				q->status = EMPTY_L;
			}else{ // k %2 == 1 
				q->status = EMPTY_R;
			}
			q->next = list[ i+1 ] ; // add the combined node to upper size	
			list[ ++i ] = q ;
			if(count > 0){
				freeFirst((p->size)-7);
			}else{
				freeNode(pre1 ,p );
			}  
			pre1 = q ;
			p = q ;
		}else if( flag == 3 ){// I found empty node in the left 
			k = q->ptr  / (powOfTwo(p->size + 1)  )  ;	
			freeNode(pre1 ,p ); // free the right node 
			p = (struct node *)getNode();	
			p->size = (q->size) + 1 ;
			p->ptr  = q->ptr ;	
			if( k % 2 == 0){				
				p->status = EMPTY_L;					
			}else{ // k %2 == 1				
				p->status = EMPTY_R;				
			}
			p->next = list[ i+1 ] ; // add the combined node to upper size	
			list[ ++i ] = p;			
			if(count > 0){
				freeFirst((q->size)-7);
			}else{
				freeNode(pre2 ,q );
			}				 
			pre1 = list[ i ] ;
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

struct node*  sortList(struct node* l){  // function to sort the given list 
// stop codition ( at the end of the list)
if(l == NULL || l->next == NULL)
    return l;  // the list is sorted.
/*Now what I want to do is every time put the largest node first and make recursion for the rest of list
 ** 1- find the largest node 
 ** 2- switch the largest with the first one ; 
 ** 3- calling the function again with the sublist
*/
//1- find largest node : 
struct node *curr, *largest,*largestPrev ,*prev,* tmp;
curr = l;
largest = l;
prev = l;
largestPrev = l;
while(curr != NULL) { // find the largest in the list(pointed by largest)
        if(curr->ptr < largest->ptr) {
            largestPrev = prev;
            largest = curr;
        }
        prev = curr;
        curr = curr->next;
    }

//2- switching firt node and largest node : 
if(largest != l){
    largestPrev->next = l;
    tmp = l->next;
    l->next = largest->next;
    largest->next = tmp;
}
// now largest is the first node of the list.
// calling the function again with the sub list :
largest->next = sortList(largest->next);
return largest;
}

struct node*  data(int listNumber , int nodeNumber ){ // function return pointer to the desired node in the desired list
	struct node *p ;
	int counter = 0 ;
	p = list[listNumber];
		while(p  != NULL){
			counter++ ;
			if(counter == nodeNumber){
				return p ;
			}
			p = p->next ;
		}
	return NULL ; 	
}
void bprint()
{
	struct node *p ;
	int i ,k ,j , counters[18] ,numOfItems = 0 ;		
	struct node  array[18]  ;            // this array is just for printing 	

	// iterate throgh all the lists and sort them
	for( i = 1 ; i <= s- 8 ; i++){ 
		list[i] = sortList(list[i]); ; 
	}
	// initialize the array
	for( i = 0 ; i < 18 ; i++){
		array[i].status = 0;
		array[i].size   = 0 ;
		array[i].ptr    = 2147483647 ; // max number can store in 32 bit

	}	
	/*get one node from every list to the array
	 ** insert this node in its correct place in order in the array
	 */

	for( i = 1 ; i < s-7  ; i++){
		p = (struct node* )data(i , 1); //get the first node from the list		
		counters[i] = 1 ;
		if(p != NULL){  // if there exist a node in the list with that number (counters[i])
			k = 0 ; 
			numOfItems++ ;			
			while(k < numOfItems && ((int)p->ptr) > ((int)array[k].ptr ))
			 k++ ;
			if(k < numOfItems ){
				for( j = numOfItems ; j >= k  ; j--){ // shift the elements in the array to the right
					array[j].ptr = array[j-1].ptr ;
					array[j].size = array[j-1].size ;
					array[j].status = array[j-1].status ;

				}
				array[k].ptr = p->ptr ; // insert the node 
				array[k].size = p->size ;
				array[k].status = p->status ;
			}
		}
	}	
	/* Now the array contain node from every list 
	 ** counters contain the order of the item in his parent list
	*/
	 printf("  Size Of Chunk   Status     Relative Address\n");
	 printf("---------------------------------------------\n");
	while(numOfItems > 0 ){

		printf("%10d       %-8s    %10d\n", powOfTwo(array[0].size), array[0].status == 1 || array[0].status == 0 ? "  FREE" : "ALOCATED" ,array[0].ptr); // printing the minimum address in the array
		counters[array[0].size - 7] = counters[array[0].size - 7] + 1;
		p  = (struct node* )data((int)array[0].size - 7  , (int)counters[array[0].size - 7] ); // get new node from the same list 		
		if(p != NULL ){
			k = 1; 
			while( k < numOfItems && ((int)p->ptr) > ((int)array[k].ptr)){  
				array[k-1].ptr = array[k].ptr ;        // shift the elements in the array to the left for insertion
				array[k-1].size = array[k].size ;
				array[k-1].status = array[k].status ;
				k++ ;
			}
				array[k-1].ptr = p->ptr ;
				array[k-1].size = p->size ;
				array[k-1].status = p->status ;			
		}else{
			for( j = 0 ; j < numOfItems-1  ; j++){ // shift the elements in the array to the left
					array[j].ptr = array[j+1].ptr ;
					array[j].size = array[j+1].size ;
					array[j].status = array[j+1].status ;
				}
			numOfItems--; 
		}

	}
	/*for( i = 1 ; i <= s- 8 ; i++){
		p = list[i];
		while(p  != NULL){
			printf("%d %d %lld\n", p->size, p->status, p->ptr);
			p = p->next ;
		}		
	}	*/
	return;
}



