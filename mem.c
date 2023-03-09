/* mem.c A template
 * My Name is Cameron Burroughs
 * My User ID is C17134631
 * Lab4: Dynamic Memory Allocation
 * ECE 2230, Fall 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"

// Global variables required in mem.c only
// NEVER use DummyChunk in your allocation or free functions!!
static mem_chunk_t DummyChunk = {0, &DummyChunk};
static mem_chunk_t * Rover = &DummyChunk;   // one time initialization
static int NumSbrkCalls = 0;
static int Pages = 0;

// private function prototypes
void mem_validate(void);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer 
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate 
 * the program.
 *
 * You can update this function to match your design.  But the method
 * to test sbrk much not be changed.  
 */
mem_chunk_t *morecore(int new_bytes) 
{
    char *cp;
    mem_chunk_t *new_p;
    // preconditions that must be true for all designs
    assert(new_bytes % PAGESIZE == 0 && new_bytes > 0);
    assert(PAGESIZE % sizeof(mem_chunk_t) == 0);
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  /* no space available */
        return NULL;
    new_p = (mem_chunk_t *) cp;
    new_p->size_units = new_bytes/sizeof(mem_chunk_t);
    // You should add some code to count the number of calls
    // to sbrk, and the number of pages that have been requested
    NumSbrkCalls++; 
    Pages += new_bytes/PAGESIZE;
    return new_p;
}

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.  
 *
 * This function assumes that the Rover pointer has already been 
 * initialized and points to some memory block in the free list.
 */
void Mem_free(void *return_ptr)
{
    // precondition
    assert(Rover != NULL && Rover->next != NULL);

    // moves p to header
    mem_chunk_t *p = (mem_chunk_t*) return_ptr;
    p--;

    if(!Coalescing) {	
        p->next = Rover->next; 
        Rover->next = p;
        p = NULL;
        Rover = Rover->next;
    } else {
	mem_chunk_t *start, *prev;
	start = Rover;
	prev = Rover;
	Rover = Rover->next;

	do { 
	    if(p > prev && p < Rover) { // middle insert
		// if fits perfectly between two locations
        	if(((prev + prev->size_units) == p) && ((p + p->size_units) == Rover)){
		    prev->next = Rover->next;
		    prev->size_units += (p->size_units + Rover->size_units);
		    p = NULL;
		    Rover = Rover->next;
		// if fits right after a pointer
		} else if((prev + prev->size_units) == p) {
		    prev->size_units += p->size_units;
	            p = NULL; 
		// if fits right before a pointer
		} else if((p + p->size_units) == Rover) {
		    p->next = Rover->next;
		    prev->next = p;
		    p->size_units += Rover->size_units;
		    p = NULL;
		// goes between the two pointers, but not contiguous.
		} else {
		    prev->next = p;
		    p->next = Rover;
		    p = NULL;
		}
	    } else if( p > Rover && Rover->next->size_units == 0) { // end insert
		// if fits right after end/ wrap pointer
		if((prev + prev->size_units) == p) {
		    prev->size_units += p->size_units;
		    p = NULL;
		// special case for wrap with two items in list
		} else if ((Rover + Rover->size_units) == p) {
		    Rover->size_units += p->size_units;
		    p = NULL;  
		// goes at end, but not contiguous
		} else {
		    p->next = Rover->next; 
		    Rover->next = p;
		    p = NULL;
	        }  
	    }
	    prev = Rover;
	    Rover = Rover->next;
        } while (Rover != start->next && p != NULL);
    }
}

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The memory is uninitialized.
 *
 * This function assumes that there is a Rover pointer that points to
 * some item in the free list.  
 */
void *Mem_alloc(const int nbytes)
{
    // precondition
    assert(nbytes > 0);
    assert(Rover != NULL && Rover->next != NULL);

    mem_chunk_t *p = NULL;
    mem_chunk_t *q = NULL;
    mem_chunk_t *candidate = NULL;
    mem_chunk_t *prev;
    mem_chunk_t *start;
    mem_chunk_t *new_page;
    
    int diff; 
    int flag = 0;
    int numPages;
    int nunits = ((nbytes/sizeof(mem_chunk_t)) + (nbytes%sizeof(mem_chunk_t) ? 1 : 0) + 1);

    if(SearchPolicy == FIRST_FIT) {
	// first fit sorts through availiable memory and returns a pointer to
	// the first block of memory with nbytes or more.
	start = Rover;
	prev = start;
	Rover = Rover->next;

	do { 	
	    if(Rover->size_units > nunits) {
		// break into two blocks
		Rover->size_units -= nunits; //subtracts memory from block
		p = Rover + Rover->size_units; // assigns pointer to new block
		p->size_units = nunits;	// setting headrer for new block;
		p->next = NULL;
	    } else if( Rover->size_units == nunits) {
		// remove block
		p = Rover; 
		Rover = Rover->next;
		prev->next = Rover;
		p->next = NULL;
	    } // else, move to next block
	    prev = Rover;
	    Rover = Rover->next;
	} while(Rover != start && p == NULL);

	if(p == NULL) {
	    // allocate new page
	    // makes requested units a multiple of PAGESIZE
	    numPages = ((nbytes + sizeof(mem_chunk_t))/PAGESIZE) + ((nbytes + sizeof(mem_chunk_t))%PAGESIZE ? 1 : 0);
	    new_page = morecore(numPages*PAGESIZE);

	    if(new_page == NULL) return NULL;  // only NULL is morecore fails
 	    // not an empty list, find place to insert new page
            if(Rover != Rover->next){  
	        while(!flag){
		    prev = Rover;
		    Rover = Rover->next;
		    flag = (Rover->next > new_page && Rover < new_page); // middle
		    flag = flag || (Rover->next->size_units == 0 && Rover < new_page); // end
		}
	    } 
	    // if exactly one page, return page
	    if(nunits == (PAGESIZE/sizeof(mem_chunk_t))){
	        p = new_page;
		p->size_units = nunits;
		p->next = NULL;
	    } else { //add page to list, return requested bytes
	        new_page->next = Rover->next;
	        Rover->next = new_page;	    

	        new_page->size_units -= nunits;
	        p = new_page + new_page->size_units;
	        p->size_units = nunits;
	        p->next = NULL;
	    }
	}
	q = p + 1;
    } else if(SearchPolicy == BEST_FIT) {
	start = Rover; 
	prev = start; 
	Rover = Rover->next;

	do {  
	    if( Rover->size_units == nunits) {
		// remove block
		p = Rover; 
		Rover = Rover->next;
		prev->next = Rover;
		p->next = NULL;
	    } else if(Rover->size_units > nunits) {
		// diff initialized when candidiate == NULL
		if(candidate == NULL || ((Rover->size_units - nunits) < diff)) {
		    diff = Rover->size_units - nunits;
		    candidate = Rover;
		}
	    }
            prev = Rover; 
	    Rover = Rover-> next;
	} while(Rover != start->next && p == NULL);  // no exact copy, but a block > requested bytes
	if((p == NULL) && (candidate != NULL)) {
	    Rover = candidate; 
	    Rover->size_units -= nunits; //subtracts memory from block
    	    p = Rover + Rover->size_units; // assigns pointer to new block
	    p->size_units = nunits;	// setting header for new block;
	    p->next = NULL;
	} else if(p == NULL) {  // no block large enough, add page
	    numPages = ((nbytes + sizeof(mem_chunk_t))/PAGESIZE) + ((nbytes + sizeof(mem_chunk_t))%PAGESIZE ? 1 : 0);
	    new_page = morecore(numPages*PAGESIZE);

	    if(new_page == NULL) return NULL; 

 	    // not an empty list, find place to insert new page
            if(Rover != Rover->next){
	        while(!flag){
		    prev = Rover;
		    Rover = Rover->next;
		    flag = (Rover->next > new_page && Rover < new_page); // middle
		    flag = flag || (Rover->next->size_units == 0 && Rover < new_page); // end
		}
	    } 
	    // if exactly one page, return page
	    if(nunits == (PAGESIZE/sizeof(mem_chunk_t))){
	        p = new_page;
		p->size_units = nunits;
		p->next = NULL;
	    } else { //add page to list, return requested bytes
	        new_page->next = Rover->next;
	        Rover->next = new_page;	    

	        new_page->size_units -= nunits;
	        p = new_page + new_page->size_units;
	        p->size_units = nunits;
	        p->next = NULL;
	    }
	}
	q = p + 1;
    }

    assert(p + 1 == q);   // +1 means one unit or sizeof(mem_chunk_t)
    assert((p->size_units-1)*sizeof(mem_chunk_t) >= nbytes);
    assert((p->size_units-1)*sizeof(mem_chunk_t) < nbytes + sizeof(mem_chunk_t));
    assert(p->next == NULL);  // saftey first!
    return q;

}

/* prints stats about the current free list
 *
 * -- number of items in the linked list including dummy item
 * -- min, max, and average size of each item (in bytes)
 * -- total memory in list (in bytes)
 * -- number of calls to sbrk and number of pages requested
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    mem_chunk_t *start;
    int  NumBlocks, M, min, max, avg;

    if(Rover == Rover->next) {
        min = 0;
        max = 0;
	NumBlocks = 1;
	M = 0;
        avg = 0;
    } else {
        // set variables
        if(Rover->size_units == 0) Rover = Rover->next; 
        start = Rover;
        M = 0;
        NumBlocks = 0;
        min = Rover->size_units;
        max = min;
	avg = 0;
        // search list for values
	do {
	    if(Rover->size_units > max) max = Rover->size_units; 
	    if(Rover->size_units < min && Rover->size_units != 0) 
		min = Rover->size_units;
            M += Rover->size_units;
	    NumBlocks++;
            Rover = Rover->next;
	} while (Rover != start); 
        avg = (NumBlocks > 1 ? M/(NumBlocks-1) : 0);
    }

    //Change units to bytes
    min *= sizeof(mem_chunk_t);
    max *= sizeof(mem_chunk_t);
    M *= sizeof(mem_chunk_t);
    avg *= sizeof(mem_chunk_t);

    printf("~~~~~~~~~~ FREE LIST MEMORY STATS ~~~~~~~~~~\n");
    printf("Items in free list (dummy block included): %d\n", NumBlocks);
    printf("Minimum Block Size: %d bytes\nMaximum Block Size: %d bytes\n", min, max);
    printf("Average Block Size: %d bytes\n", avg);
    printf("Total Memory: %d bytes\n", M);
    printf("Number of calls to sbrk(): %d\n", NumSbrkCalls);
    printf("Total Pages: %d\n", Pages); 
    if(M == (Pages*PAGESIZE))
        printf("all memory is in the heap -- no leaks are possible\n");
}

/* print table of memory in free list 
 *
 * The print should include the dummy item in the list 
 *
 * A unit is the size of one mem_chunk_t structure
 */
void Mem_print(void)
{
    // note position of Rover is not changed by this function
    assert(Rover != NULL && Rover->next != NULL);
    mem_chunk_t *p = Rover;
    mem_chunk_t *start = p;
    do {
        // example format.  Modify for your design
        printf("p=%p, size=%d (units), end=%p, next=%p %s\n", 
                p, p->size_units, p + p->size_units, p->next, 
                p->size_units!=0?"":"<-- dummy");
        p = p->next;
    } while (p != start);
    printf("\n");
    mem_validate();
    
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    // note position of Rover is not changed by this function
    assert(Rover != NULL && Rover->next != NULL);
    assert(Rover->size_units >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    int found_rover = FALSE;
    mem_chunk_t *p, *largest, *smallest;

    // for validate begin at DummyChunk
    p = &DummyChunk;
    do {
        if (p->size_units == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size_units > 0);
        }
        if (p == Rover) {
            assert(found_rover == FALSE);
            found_rover = TRUE;
        }
        p = p->next;
    } while (p != &DummyChunk);
    assert(found_dummy == TRUE);
    assert(found_rover == TRUE);

    if (Coalescing) {
        do {
            if (p >= p->next) {
                // this is not good unless at the one wrap around
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(0);   // stop and use gdb
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size_units < p->next);
            }
            p = p->next;
        } while (p != &DummyChunk);
        assert(wrapped == TRUE);
    }
}
/* vi:set ts=8 sts=4 sw=4 et: */

