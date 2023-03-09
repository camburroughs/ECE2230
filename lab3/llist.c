/* llist.c
 * Cameron Burroughs    
 * burrou5
 * ECE 2230 Fall 2020
 * MP3
 *
 * Propose: Defines public functions to access private list element
 * 		members. Handles linked list operations.
 *
 * Assumptions: Cannot see or access any 'ids_' members or functions.
 *
 * 		Uses data_t type and therefore cannot depend on
 * 		members of alert_t type as defined in datatypes.h
 *		
 *		All llist_debug_validate calls were checked before
 *		being commented out in final code. Commented calls marked 
 *		by 'TODO' for visibility in vim.
 * 
 * Bugs: N/A
 *
 */

#include <stdlib.h>
#include <assert.h>

#include <stdio.h>

#include "llist.h"        // defines public functions for list ADT

// definitions for private constants used in llist.c only

#define LLIST_SORTED    999999
#define LLIST_UNSORTED -888888

// prototypes for private functions used in llist.c only 
void llist_debug_validate(llist_t *L);
void ll_InsertionSort(llist_t *list_ptr);
llist_elem_t* ll_FindMax(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n);   /* assume m<n */
void ll_SelectionSort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n);
void ll_IterativeSelectionSort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n);
void ll_MergeSort(llist_t *list_ptr);
void ll_SplitList(llist_t* list_ptr, llist_t *L, llist_t *R);	// used in MergeSort
void ll_MergeList(llist_t *new_list, llist_t *L, llist_t *R);	// used in MergeSort

/* ----- below are the functions  ----- */

/* llist_sort takes in a pointer to a linked list, a sort type identifer, 
 * and a comparison function pointer.
 *
 * The function sends the list pointer and the comparison function to 
 * the sorting algorithm specified by sort_type.
 *
 * Before sorting, the list is set to LLIST_UNSORTED type.
 * After sorting, the list is set to LLIST_SORTED type.
 *
 */
void llist_sort(llist_t *list_ptr, int sort_type, int (*fcomp)(const data_t *, const data_t *))
{
    list_ptr->compare_fun = fcomp;
    
    //satisfies llist_debug_validate if sorting fn changes
    list_ptr->ll_sorted_state = LLIST_UNSORTED;    

    if (sort_type == 1){
        ll_InsertionSort(list_ptr);
    } else if(sort_type == 2) {
        ll_SelectionSort(list_ptr, list_ptr->ll_front, list_ptr->ll_back);
    } else if(sort_type == 3) {
        ll_IterativeSelectionSort(list_ptr, list_ptr->ll_front, list_ptr->ll_back);
    } else if(sort_type == 4) {
        ll_MergeSort(list_ptr); 
    }
    
    list_ptr->ll_sorted_state = LLIST_SORTED;
    llist_debug_validate(list_ptr);	// ONLY debug call not surpressed in llist.c
}

/* ll_MergeSort is identified by sort_type 4 and recieves only a pointer
 * to the list it is sorting.
 * 
 * This function utilizes recursive calls along with two sub functions,
 * SplitList and MergeList, to quickly sort any linked list by whatever
 * order is specified in its compare_fun.
 */
void ll_MergeSort(llist_t *list_ptr)
{
    llist_t *sorted_list, *L, *R;
    

    // based on pseudo-code, if list has more than one item
    if(list_ptr->ll_front != NULL && list_ptr->ll_front->ll_next != NULL)
    {
        // constructing new lists for split and merge subfunctions
        L = llist_construct(list_ptr->compare_fun);
        R = llist_construct(list_ptr->compare_fun);
        sorted_list = llist_construct(list_ptr->compare_fun);

        ll_SplitList(list_ptr,L,R);
        ll_MergeSort(L);
        ll_MergeSort(R);
        ll_MergeList(sorted_list, L, R);
    
	// assign sorted list to list_ptr
        list_ptr->ll_front = sorted_list->ll_front;
        list_ptr->ll_back = sorted_list->ll_back;
        list_ptr->ll_entry_count = sorted_list->ll_entry_count;
    
	// destruct second temporary list
        sorted_list->ll_entry_count = 0;
        sorted_list->ll_front = NULL;
        sorted_list->ll_back = NULL;
        llist_destruct(sorted_list); 
        // free memory
        llist_destruct(L);
        llist_destruct(R);
        
    } // else, do nothing: already sorted
    
}

/* ll_MergeList is a subroutine of MergeSort that recombines the 
 * two linked lists, L and R, into one sorted list.
 *
 * MergeList itself does not use recursive calls, however it is 
 * used in a recursive function.
 *
 * This function utilizes llist_remove and llist_insert_sorted
 * to reconstruct and sort the list.
 */
void ll_MergeList(llist_t *sorted_list, llist_t *L, llist_t *R)
{
    data_t* temp;
    // while at least one list is not empty
    while(L->ll_front != NULL || R->ll_front != NULL)
    {
	// if one list is empty, insert from other list
        if(L->ll_front == NULL) {
            temp = llist_remove(R, LLPOSITION_FRONT);
            llist_insert(sorted_list,temp, LLPOSITION_BACK);
        } else if(R->ll_front == NULL) {
            temp = llist_remove(L, LLPOSITION_FRONT);
            llist_insert(sorted_list,temp, LLPOSITION_BACK);
        // else, insert based on compare_fun (1 = closer to front)
	} else {
            int comp = sorted_list->compare_fun(L->ll_front->data_ptr,R->ll_front->data_ptr);
            if(comp == 1) {
                temp = llist_remove(L, LLPOSITION_FRONT);
                llist_insert(sorted_list,temp, LLPOSITION_BACK);
            } else {
                temp = llist_remove(R, LLPOSITION_FRONT);
                llist_insert(sorted_list,temp, LLPOSITION_BACK);
            }
        }
    }
}

/* ll_SplitList is a subroutine of MergeSort that splits the unsorted 
 * linked list into two smaller lists, L and R.
 *
 * SplitList itself does not use recursive calls, however it is 
 * used in a recursive function.
 *
 * This function utilizes two rovers, MidPoint and EndPoint, to find
 * the middle of the list. The second rover moves twice as fast as
 * the first rover, so when the second rover hits the end of the 
 * list, the first rover hits the middle. The first rover is then 
 * used as a mid point to split the list into L and R.
 */
void ll_SplitList(llist_t* list_ptr, llist_t *L, llist_t *R)
{
    // rovers starting at first and second nodes.
    llist_elem_t *MidPoint = list_ptr->ll_front;
    llist_elem_t *EndPoint = list_ptr->ll_front->ll_next;

    int count = list_ptr->ll_entry_count;
    
    // finding mid point
    while(EndPoint != NULL)
    {
        EndPoint = EndPoint->ll_next;
        if(EndPoint != NULL)
        {
            EndPoint = EndPoint->ll_next;
            MidPoint = MidPoint->ll_next;
        }
    }
    // spliting list into L and R halves
    L->ll_front = list_ptr->ll_front;
    L->ll_back = MidPoint;
    R->ll_front = MidPoint->ll_next;
    R->ll_back = list_ptr->ll_back;
    L->ll_back->ll_next = NULL;
    R->ll_front->ll_prev = NULL;

    // updating entry count   
    L->ll_entry_count = (count/2) + (count%2);
    R->ll_entry_count = (count/2);

    // avoids dangling pointers
    MidPoint = NULL;
    EndPoint = NULL;

}
/* ll_IterativeSelectionSort takes in a list pointer, and pointers
 * to the first and last elements in a list.
 * 
 * The function utilizes a do while loop to iteratively sort the
 * elements in the list based on its compare_fun.
 *
 * The algorithm finds the position of the element between m and n
 * that belongs closest to the front of the list. Once found, it is
 * moved to position m and m is incremented forward in the list.
 * 
 * assumes m is closer to front than n
 * 
 * depnding on compare_fun, MaxPosition could actually be pointing 
 * the smallest element between m and n, or the largest, however 
 * regardless of the compare_fun, it points to the element that
 * should be closest to the front between m and n
 */ 
void ll_IterativeSelectionSort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n)
{
   llist_elem_t *MaxPosition, *i;
   data_t* temp;

   // if more than one item to sort
   while (m != n) {
   
      i = m; 
      MaxPosition = m;

      do {
         i = i->ll_next;
         if (list_ptr->compare_fun(i->data_ptr,MaxPosition->data_ptr) == 1 ) MaxPosition = i;
      } while (i != n);
   
      temp = m->data_ptr; m->data_ptr = MaxPosition->data_ptr; MaxPosition->data_ptr = temp;
   
      m = m->ll_next;
   }
}

/* ll_SelectionSort takes in a list pointer, and pointers
 * to the first and last elements in a list.
 * 
 * The function utilizes a recursive call and a subroutine, FindMax,
 * to sort the elements in the list based on its compare_fun.
 *
 * The algorithm finds the position of the element between m and n
 * that belongs closest to the front of the list. Once found, it is
 * moved to position m and m is incremented forward in the list 
 * before a recursive call.
 * 
 * assumes m is closer to front than n
 * 
 * depnding on compare_fun, MaxPosition could actually be pointing 
 * the smallest element between m and n, or the largest, however 
 * regardless of the compare_fun, it points to the element that
 * should be closest to the front between m and n
 */ 
void ll_SelectionSort(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n)
{
   llist_elem_t   *MaxPosition;    
   data_t   *temp;    // temp is used to exchange items in list_ptr 
   
   // if more than one item to sort
   if (m != n) {               
   
         MaxPosition = ll_FindMax(list_ptr,m,n);
      
         // Exchange m and MaxPosition 
         temp = m->data_ptr;
         m->data_ptr = MaxPosition->data_ptr;
         MaxPosition->data_ptr = temp;
         
	 // recursion call    
         ll_SelectionSort(list_ptr, m->ll_next, n);
   }
}

/* ll_FindMax is a subroutine used in ll_SelectionSort that takes 
 * in two element pointers and a list pointer. 
 *
 * The function utilizes the compare_fun to compare the elements between 
 * m and n.
 *
 * assumes m < n 
 * 
 * return value: 
 *	a pointer to the value that belongs closest to the front of the list
 * 	between m and n is returned.
 */
llist_elem_t* ll_FindMax(llist_t *list_ptr, llist_elem_t *m, llist_elem_t *n)
{
   llist_elem_t* i = m;      
   llist_elem_t* j = m;    
            
                     
   do {
      i = i->ll_next;            
      if (list_ptr->compare_fun(i->data_ptr,j->data_ptr) == 1) {         
         j = i;              
      }
   } while (i != n);        

   return j;  // returns position of element to be closest to m                
}                                                

/* ll_InsertionSort takes in a pointer to a list and uses functions
 * llist_remove and llist_insert_sorted to remove items from one 
 * list and sort them by inserting them into a new, sorted list 
 * based on the list's compare_fun.
 *
 * After all items are removed from the original list, the header
 * pointers are adjusted to point to the sorted list, and the 
 * second list header is destructed.
 */
void ll_InsertionSort(llist_t *list_ptr)
{
    llist_t *sorted_list;
    data_t *temp;
    sorted_list = llist_construct(list_ptr->compare_fun);
 
    while(list_ptr->ll_front != NULL)
    {
        temp = llist_remove(list_ptr, LLPOSITION_FRONT);
        llist_insert_sorted(sorted_list, temp);
    }
    
    list_ptr->ll_front = sorted_list->ll_front;
    list_ptr->ll_back = sorted_list->ll_back;
    list_ptr->ll_entry_count = sorted_list->ll_entry_count;
    
    sorted_list->ll_entry_count = 0;

    sorted_list->ll_front = NULL;
    sorted_list->ll_back = NULL;
    llist_destruct(sorted_list); 
}

/* Obtains a pointer to an element stored in the specified list, at the
 * specified list position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from llist_construct.
 *
 * pos_index: position of the element to be accessed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LLPOSITION_FRONT and LLPOSITION_BACK
 *
 * return value: pointer to element accessed within the specified list.  A
 * value NULL is returned if the pos_index does not correspond to an element in
 * the list.
 */
data_t * llist_access(llist_t *list_ptr, int pos_index)
{
    int count = 0;
    llist_elem_t *L;
 
    // debugging function to verify that the structure of the list is valid
//TODO    llist_debug_validate(list_ptr);

    /* handle special cases.
     *   1.  The list is empty
     *   2.  Asking for the head 
     *   3.  Asking for the tail
     *   4.  specifying a position that is out of range.  This is not defined
     *       to be an error in this function, but instead it is assumed the 
     *       calling function correctly specifies the position index
     */
    if (llist_entries(list_ptr) == 0) {
        return NULL;  // list is empty
    }
    else if (pos_index == LLPOSITION_FRONT || pos_index == 0) {
        return list_ptr->ll_front->data_ptr;  // asking for head
    }
    else if (pos_index == LLPOSITION_BACK || pos_index == llist_entries(list_ptr) - 1) {
        return list_ptr->ll_back->data_ptr;  // asking for tail
    }
    else if (pos_index < 0 || pos_index >= llist_entries(list_ptr))
        return NULL;   // does not correspond to position in list

    // loop through the list to find correct position index
    L=list_ptr->ll_front;
    for (count = 0; count < pos_index; count++) {
        L = L->ll_next;
    }
    // already verified that pos_index should be valid so L better not be null
    assert(L != NULL);
    assert(L->data_ptr != NULL);
    return L->data_ptr;
}

/* Allocates a new, empty list 
 *
 * If the comparison function is NULL, then the list is unsorted.
 *
 * Otherwise, the list is initially assumed to be sorted.  Note that if 
 * list_insert is used the list is changed to unsorted.  
 *
 * The field sorted can only take values LLIST_SORTED or LLIST_UNSORTED
 *
 * Use llist_destruct to remove and deallocate all elements on a list 
 * and the header block.
 */
llist_t * llist_construct(int (*fcomp)(const data_t *, const data_t *))
{
    llist_t *L;
    L = (llist_t *) malloc(sizeof(llist_t));
    L->ll_front = NULL;  
    L->ll_back = NULL;
    L->ll_entry_count = 0;
    L->compare_fun = fcomp;
    if (fcomp == NULL)
        L->ll_sorted_state = LLIST_UNSORTED;
    else
        L->ll_sorted_state = LLIST_SORTED;

//TODO    llist_debug_validate(L);
    return L;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 * 
 * fcomp: function to test if elem_ptr is a match to an item in the list.
 *        Returns 0 if they match and any value not equal to 0 if they do not match.
 *
 * NOTICE: pos_index is returned and is not an input value!
 *
 * The function returns a pointer to the matching element with lowest index if
 * a match if found.  If a match is not found the return value is NULL.
 *
 * The function also returns the integer position of matching element with the
 *           lowest index.  If a matching element is not found, the position
 *           index that is returned should be -1. 
 *
 * pos_index: used as a return value for the position index of matching element
 */
data_t * llist_elem_find(llist_t *list_ptr, data_t *elem_ptr, int *pos_index,
        int (*fcomp)(const data_t *, const data_t *))
{
//TODO    llist_debug_validate(list_ptr);

    //declaring pointers
    llist_elem_t *tmp;
    data_t* ret;

    //initializing variables
    tmp = list_ptr->ll_front;
    ret = NULL;
    (*pos_index) = 0;

    // finds index and sets ret to matching element
    while(tmp != NULL){
        if(!fcomp(tmp->data_ptr,elem_ptr)){
	    ret = tmp->data_ptr;
	    tmp = NULL;
	} else {
	    (*pos_index)++; 
	    tmp = tmp->ll_next;
	}
    }
		         
    if(ret == NULL) {*pos_index = -1;}   // if no match found, return -1 index
    
    return ret;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 */
void llist_destruct(llist_t *list_ptr)
{
    // the first line must validate the list 
//TODO    llist_debug_validate(list_ptr);

    // creating temporary pointers
    llist_elem_t* cur_elem = list_ptr->ll_front;
    llist_elem_t* next_elem;
    data_t* cur_data;

    // free elements and data
    while(cur_elem != NULL) {
	next_elem = cur_elem->ll_next;
	cur_data = cur_elem->data_ptr;
	free(cur_data);
	free(cur_elem);
	cur_elem = next_elem;
	list_ptr->ll_entry_count--;
    }

    // free list
    list_ptr->ll_front = NULL;
    list_ptr->ll_back = NULL;
    free(list_ptr);
    list_ptr = NULL;

}

/* Inserts the specified data element into the specified list at the specified
 * position.
 *
 * llist_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * pos_index: numeric position index of the element to be inserted into the 
 *            list.  Index starts at 0 at head of the list, and incremented by 
 *            one until the tail is reached.  The index can also be equal
 *            to LLPOSITION_FRONT or LLPOSITION_BACK (these are special negative 
 *            values use to provide a short cut for adding to the head
 *            or tail of the list).
 *
 * If pos_index is greater than the number of elements currently in the list, 
 * the element is simply appended to the end of the list (no additional elements
 * are inserted).
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the llist_ptr->ll_sorted_state must be equal 
 * to LLIST_UNSORTED.
 */
void llist_insert(llist_t *list_ptr, data_t *elem_ptr, int pos_index)
{
    assert(list_ptr != NULL);
 
    llist_elem_t *tmp, *new;
    int count = 0;
    int max = llist_entries(list_ptr);
    
    new = (llist_elem_t*)malloc(sizeof(llist_elem_t));
    new->data_ptr = elem_ptr;
    new->ll_prev = NULL;
    new->ll_next = NULL;

    tmp = list_ptr->ll_front;
    
    if((list_ptr->ll_front == NULL) && (list_ptr->ll_back == NULL)) {	// if list is empty 
	list_ptr->ll_front = new;
	list_ptr->ll_back = new;
    } else if(pos_index == LLPOSITION_FRONT || pos_index == 0){ // add to front
	new->ll_next = list_ptr->ll_front;
	list_ptr->ll_front->ll_prev = new;
	list_ptr->ll_front = new;
	list_ptr->ll_front->ll_prev = NULL;
    } else if(pos_index == LLPOSITION_BACK || pos_index >= max) {	// if invalid index, add to end
	list_ptr->ll_back->ll_next = new;
	new->ll_prev = list_ptr->ll_back;
	list_ptr->ll_back = new;
	list_ptr->ll_back->ll_next = NULL;
    } else { 
    	// locates element before index	    
	for(count = 0; count < pos_index - 1; count++) {
   	    tmp = tmp->ll_next;
	}

	// inserts at pos_index
	new->ll_next = tmp->ll_next;
	new->ll_prev = tmp;
	tmp->ll_next->ll_prev = new;
	tmp->ll_next = new;
    }

    list_ptr->ll_entry_count++;		// increment entries

    // the last three lines of this function must be the following 
    if (list_ptr->ll_sorted_state == LLIST_SORTED) 
        list_ptr->ll_sorted_state = LLIST_UNSORTED;
//TODO    llist_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the compare_fun function pointer found in the header block.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use llist_insert_sorted, the list preserves its sorted nature.
 *
 * If you use llist_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call llist_insert_sorted, this subroutine
 * should generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the list head than B
 *   -1: B should be closer to the list head than A
 *    0: A and B are equal in rank
 *
 * Note: if the element to be inserted is equal in rank to an element already
 *       in the list, the newly inserted element will be placed after all the 
 *       elements of equal rank that are already in the list.
 */
void llist_insert_sorted(llist_t *list_ptr, data_t *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_sorted_state == LLIST_SORTED);

    // initialize new element
    llist_elem_t *tmp, *new;
    int max = llist_entries(list_ptr);
    new = (llist_elem_t*)malloc(sizeof(llist_elem_t));
    new->data_ptr = elem_ptr;
    new->ll_prev = NULL;
    new->ll_next = NULL;

    tmp = list_ptr->ll_front;
    
    if(max == 0 && list_ptr->ll_front == NULL && list_ptr->ll_back == NULL) {	// if list is empty 
	list_ptr->ll_front = new;
	list_ptr->ll_back = new;
    } else if(list_ptr->compare_fun(elem_ptr, list_ptr->ll_front->data_ptr) == 1) {
	// adds to front
	new->ll_next = list_ptr->ll_front;
	list_ptr->ll_front->ll_prev = new;
	list_ptr->ll_front = new;
    } else if((list_ptr->compare_fun(elem_ptr,list_ptr->ll_back->data_ptr)) != 1) {
	// adds to back
	new->ll_prev = list_ptr->ll_back;
	list_ptr->ll_back->ll_next = new;
	list_ptr->ll_back = new;
    } else {		// adds to middle (sorted)
	for(;(list_ptr->compare_fun(elem_ptr,tmp->data_ptr)) != 1; tmp = tmp->ll_next);
	
	// insert new	
	tmp->ll_prev->ll_next = new;
	new->ll_prev = tmp->ll_prev;
	tmp->ll_prev = new;
	new->ll_next = tmp;
    }

    list_ptr->ll_entry_count++;		// increment entries
    // the last line checks if the new list is correct 
//TODO    llist_debug_validate(list_ptr);
}

/* Removes an element from the specified list, at the specified list position,
 * and returns a pointer to the element.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * pos_index: position of the element to be removed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LLPOSITION_FRONT and LLPOSITION_BACK
 *
 * Attempting to remove an element at a position index that is not contained in
 * the list will result in no element being removed, and a NULL pointer will be
 * returned.
 */
data_t * llist_remove(llist_t *list_ptr, int pos_index)
{
//TODO    llist_debug_validate(list_ptr);
    
    // declare pointers
    llist_elem_t *tmp, *tmpNext, *tmpPrev;
    data_t* ret;

    //initialize variables
    tmp = list_ptr->ll_front;
    ret = NULL;

    int max = llist_entries(list_ptr);
    
    if((max == 0) || (pos_index >= max)){ //list empty or index out of bounds
	ret = NULL;
    } else if( pos_index == 0 || pos_index == LLPOSITION_FRONT) {
	// remove from front
	ret = list_ptr->ll_front->data_ptr;
	if(list_ptr->ll_front->ll_next != NULL) {
	    list_ptr->ll_front->ll_next->ll_prev = NULL;
	}
	tmp = list_ptr->ll_front->ll_next;
	free(list_ptr->ll_front);
	list_ptr->ll_front = tmp;
	
	list_ptr->ll_entry_count--;
    
    } else if( pos_index == (max-1) || pos_index == LLPOSITION_BACK) {
	// remove from back
	ret = list_ptr->ll_back->data_ptr;
	
	if(list_ptr->ll_back->ll_prev != NULL) {
	    list_ptr->ll_back->ll_prev->ll_next = NULL;
	}

	tmp = list_ptr->ll_back->ll_prev;
	list_ptr->ll_back->ll_prev = NULL;
	list_ptr->ll_back->data_ptr = NULL;
	free(list_ptr->ll_back);
	list_ptr->ll_back = tmp;
	
	list_ptr->ll_entry_count--;
    
    } else {
    	// locates element	    
	for(int count = 0; count < pos_index; count++) {
   	    tmp = tmp->ll_next;
	}

	ret = tmp->data_ptr;

	// removes node
	tmpNext = tmp->ll_next;
	tmpPrev = tmp->ll_prev;
	tmpNext->ll_prev = tmpPrev;
	tmpPrev->ll_next = tmpNext;

	// avoids dangling ptr
	tmp->ll_next = NULL;
	tmp->ll_prev = NULL;
	tmp->data_ptr = NULL;
	free(tmp);
	list_ptr->ll_entry_count--;
    }
	
    tmp = NULL;	  // avoids dangling pointer
	
    if(list_ptr->ll_entry_count == 0){
        list_ptr->ll_front = NULL;
        list_ptr->ll_back = NULL;
    }

    /* fix the return value */
    return ret;
}

/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 */
int llist_entries(llist_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->ll_entry_count >= 0);
    return list_ptr->ll_entry_count;
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void llist_debug_validate(llist_t *L)
{
    llist_elem_t *N;
    int count = 0;
    assert(L != NULL);
    if (L->ll_front == NULL)
        assert(L->ll_back == NULL && L->ll_entry_count == 0);
    if (L->ll_back == NULL)
        assert(L->ll_front == NULL && L->ll_entry_count == 0);
    if (L->ll_entry_count == 0)
        assert(L->ll_front == NULL && L->ll_back == NULL);
    assert(L->ll_sorted_state == LLIST_SORTED || L->ll_sorted_state == LLIST_UNSORTED);

    if (L->ll_entry_count == 1) {
        assert(L->ll_front == L->ll_back && L->ll_front != NULL);
        assert(L->ll_front->ll_next == NULL && L->ll_front->ll_prev == NULL);
        assert(L->ll_front->data_ptr != NULL);
    }
    if (L->ll_front == L->ll_back && L->ll_front != NULL)
        assert(L->ll_entry_count == 1);
    if (L->ll_entry_count > 1) {
        assert(L->ll_front != L->ll_back && L->ll_front != NULL && L->ll_back != NULL);
        N = L->ll_front;
        assert(N->ll_prev == NULL);
        while (N != NULL) {
            assert(N->data_ptr != NULL);
            if (N->ll_next != NULL) assert(N->ll_next->ll_prev == N);
            else assert(N == L->ll_back);
            count++;
            N = N->ll_next;
        }
        assert(count == L->ll_entry_count);
    }
    if (L->ll_sorted_state == LLIST_SORTED && L->ll_front != NULL) {
        N = L->ll_front;
        while (N->ll_next != NULL) {
            assert(L->compare_fun(N->data_ptr, N->ll_next->data_ptr) != -1);
            N = N->ll_next;
        }
    }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
