/* lab4.c
 * Cameron Burroughs
 * C17134631
 * Lab4: Dynamic Memory Allocation
 * Version: 1
 * ECE 2230, Fall 2020
 *
 * This file contains drivers to test the mem.c package for dynamic memory
 * allocation.
 * 
 * The following parameters can be set on the command line.  If not set,
 * default values are used.  
 *
 * If different options are implemented for the memory package, this provides a
 * simple mechanism to change the options.  
 *
 * -f best|first  search policy to find memory block (first by default)
 * -c             turn on coalescing (off by default)
 *
 * General options for all test drivers
 * -s 19283  random number generator seed
 * -v        verbose
 *
 * The Unit test drivers.  Two example drivers are provided.  You MUST
 *           add one or more additional unit drivers for more detailed testing
 * 
 * -u 0      Tests one allocation for 16 bytes
 * -u 1      Tests 4 allocations including a new page
 *           The student must update this driver to match the details of
 *           his or her design.
 *
 * -u ?      The student is REQUIRED to add additional drivers
 *
 * The equilibrium test driver.  See comments with equilibriumDriver below for
 * additional options.
 * -e        run equilibrium test driver
 *
 * To test the equilibrium driver with the system malloc/free isntead
 * of your implementation of a heap use the option
 * -d        Use system malloc/free to verify equilibrium dirver and list ADT
 *           work as expected
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <malloc.h>    // OSX users may need to comment out this include
#include <time.h>

#include "llist.h"
#include "mem.h"

// Global variables first defined in mem.h 
int SearchPolicy = FIRST_FIT;
int Coalescing = FALSE;

// structure for equilibrium driver parameters 
typedef struct {
    int Seed;
    int Verbose;
    int EquilibriumTest;
    int WarmUp;
    int Trials;
    int AvgNumInts;
    int RangeInts;
    int SysMalloc;
    int UnitDriver;
} driver_params;

// prototypes for functions in this file only 
void getCommandLine(int argc, char **argv, driver_params *ep);
void equilibriumDriver(driver_params *ep);

int main(int argc, char **argv)
{
    driver_params dprms;
    getCommandLine(argc, argv, &dprms);
    printf("Seed: %d\n", dprms.Seed);
    srand48(dprms.Seed);

    // The major choices: search policy and coalescing option 
    if (SearchPolicy == BEST_FIT) printf("Best-fit search policy");
    else if (SearchPolicy == FIRST_FIT) printf("First-fit search policy");
    else {
        fprintf(stderr, "Error with undefined search policy\n");
        exit(1);
    }
    if (Coalescing == TRUE) printf(" using coalescing\n");
    else if (Coalescing == FALSE) printf(" without coalescing\n");
    else {
        fprintf(stderr, "Error specify coalescing policy\n");
        exit(1);
    }

    if (dprms.UnitDriver == 0)
    {
        // unit driver 0: basic test with one allocation and free
        printf("\n----- Begin unit driver 0 -----\n");
        char *string;
        const char msg[] = "hello world 15c";
        int len = strlen(msg);
        // add one for null character at end of string 
        string = (char *) Mem_alloc((len+1) * sizeof(char));
        strcpy(string, msg);
        printf("string length=%d\n:%s:\n", len, string);
        printf("\nFree list after first allocation\n");
        Mem_stats();
        Mem_print();
        Mem_free(string);
        printf("\nFree list after first free\n");
        printf("unit driver 0 has returned all memory to free list\n");
        Mem_stats();
        Mem_print();
        string = NULL;
        printf("\n----- End unit driver 0 -----\n");
    }
    else if (dprms.UnitDriver == 1)
    {
        printf("\n----- Begin unit driver 1 -----\n");
        /* You MUST create at least one new unit driver.

         Here is an example of a unit test driver.

         This is a specific example of the general statement made in the
         MP4.pdf file, in the section "Testing and performance evaluation".
         This test makes three allocations from the free list with the goal
         of making the third allocation the correct size so that the free
         list is left empty. To get this test to work with your code you need
         to determine two values.
         */

        int unit_size = sizeof(mem_chunk_t);
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and one unit is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4;
        int num_bytes_1, num_bytes_2, num_bytes_3;
        int num_bytes_4;

        // allocate 1st pointer to 1/8 of a page
        num_bytes_1 = (units_in_first_page/8 - 1)*unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();

        // allocate for 2nd pointer to 1/2 of a page
        num_bytes_2 = (units_in_first_page/2 - 1)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("second: %d bytes (%d units) at p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print();

        // allocate remaining memory in free list
        num_bytes_3 = units_in_first_page - num_bytes_1/unit_size 
            - num_bytes_2/unit_size - 3;
        num_bytes_3 *= unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("third: %d bytes (%d units) at p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print();
        printf("unit driver 1: above Mem_print shows empty free list\n");

        // allocate for 4th pointer to 1/4 a page when free list is empty
        num_bytes_4 = (units_in_first_page/4 - 1)*unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("fourth: %d bytes (%d units) at p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print();

        // next put the memory back into the free list:

        printf("first free of 1/8 a page p=%p \n", p1);
        Mem_free(p1);
        Mem_print();

        printf("second free of 3/8 a page p=%p \n", p3);
        Mem_free(p3);
        Mem_print();

        printf("third free of 1/2 a page p=%p \n", p2);
        Mem_free(p2);
        Mem_print();
        printf("fourth free of 1/4 a page p=%p\n", p4);
        Mem_free(p4);
        printf("unit driver 1 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 1 -----\n");
    }
    else if (dprms.UnitDriver == 2)
    {
	/* This driver tests page alocation bounds and inserts */	

        printf("\n----- Begin unit driver 2 -----\n");

        int unit_size = sizeof(mem_chunk_t);
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and one unit is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3;
        int num_bytes_1, num_bytes_2, num_bytes_3;

        // allocates and free 1 page units of space
        num_bytes_1 = (units_in_first_page)*unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
        
        // allocates and free 1 page  + 1 units of space
        num_bytes_2 = (units_in_first_page + 1)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print();

        // allocates and free 1 page - 1 units of space
        num_bytes_3 = (units_in_first_page - 1)*unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print();

        printf("first free of 258 units p=%p \n", p1);
        Mem_free(p1);
        Mem_print();
        
	printf("second free of 257 units p=%p \n", p2);
        Mem_free(p2);
        Mem_print();
        
	printf("third free of 256 units p=%p \n", p3);
        Mem_free(p3);
        Mem_print();

        printf("unit driver 2 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 2 -----\n");
    }
    else if (dprms.UnitDriver == 3)
    {
	/* this driver tests that an amount of bytes larger than one page 
	   will be allocated two pages. It also checks that if a user requests
	   an amount of bytes that is not a multiple of PAGESIZE or 
	   sizeof(mem_chunk_t), that it still allocates an appropriate amount. 
	   Finally, this driver tests the difference between best-fit and first-
	   fit allocation policies */	

        printf("\n----- Begin unit driver 3 -----\n");

        int unit_size = sizeof(mem_chunk_t);
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and one unit is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4;
        int num_bytes_1, num_bytes_2, num_bytes_3;
	int num_bytes_4;

        // allocates 4100 bytes (not a multiple of unit size)
        num_bytes_1 = 4100;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size + (num_bytes_1%unit_size ? 
		1 : 0), p1);
        Mem_print();
        
        // Allocating two more blocks
        num_bytes_2 = (26)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print();

        num_bytes_3 = (128)*unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print();

	// this free creates a gap
        printf("first free of 26 + 1 units p=%p \n", p2);
        Mem_free(p2);
        Mem_print();
        
	// this allocation will how the difference between first-fit
	// and best-fit
        num_bytes_4 = (14)*unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print();


	printf("second free of 4100 bytes p=%p \n", p1);
        Mem_free(p1);
        Mem_print();
        
	printf("third free of 14 units p=%p \n", p4);
        Mem_free(p4);
        Mem_print();

	printf("fourth free of 128 units p=%p \n", p3);
        Mem_free(p3);
        Mem_print();

        printf("unit driver 3 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 3 -----\n");
    }
    else if (dprms.UnitDriver == 4)
    {
	/* This driver tests the various combinations for 
	   coalescing */

        printf("\n----- Begin unit driver 4 -----\n");

        int unit_size = sizeof(mem_chunk_t);
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and one unit is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4, *p5;
        int num_bytes_1, num_bytes_2, num_bytes_3;
	int num_bytes_4, num_bytes_5;

	// five chunks allocated
        num_bytes_1 = (63)*unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
        
        num_bytes_2 = (23)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print();

        num_bytes_3 = (31)*unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print();
        
	num_bytes_4 = (89)*unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print();

	num_bytes_5 = (29)*unit_size;
        p5 = (int *) Mem_alloc(num_bytes_5);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_5, num_bytes_5/unit_size, p5);
        Mem_print();

	// first free of block contiguous with memory in free
	// expected output is for this block to combine with the 
	// end of the currrent memory in the free list
        printf("first free of 30 units p=%p \n", p5);
        Mem_free(p5);
        Mem_print();
        
	// creates a gap
	printf("second free of 24 units p=%p \n", p2);
        Mem_free(p2);
        Mem_print();
        
	// should combine with the block previously pointed to by
	// p2. The back end of p3 combines with the front of p2
	printf("third free of 32 units p=%p \n", p3);
        Mem_free(p3);
        Mem_print();
	
	// between two blocks, should combine on both ends
	printf("fourth free of 90 units p=%p \n", p4);
        Mem_free(p4);
        Mem_print();
        
	// wrap around block, should combine with the rest of memory
	// front end of p1 should combine to back end of free list
	printf("fifth free of 64 units p=%p \n", p1);
        Mem_free(p1);
        Mem_print();
        

        printf("unit driver 4 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 4 -----\n");
    }
    else if (dprms.UnitDriver == 5)
    {
	/* This function tests the rover's movement ensuring 
	   it will not cause significant fragmentation near the front 
	   of the list */

        printf("\n----- Begin unit driver 5 -----\n");

        int unit_size = sizeof(mem_chunk_t);
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and one unit is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4, *p5, *p6;
        int num_bytes_1, num_bytes_2, num_bytes_3;
	int num_bytes_4, num_bytes_5, num_bytes_6;

	// four allocations
        num_bytes_1 = (89)*unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
        
        num_bytes_2 = (63)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print();

        num_bytes_3 = (26)*unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print();

	num_bytes_4 = (111)*unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print();

	// free 64 units, should leave 3 medium blocks of memory
	printf("first free of 64 units p=%p \n", p2);
        Mem_free(p2);
        Mem_print();

	// will remove 30 from one block, the move to next
	num_bytes_5 = (29)*unit_size;
        p5 = (int *) Mem_alloc(num_bytes_5);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_5, num_bytes_5/unit_size, p5);
        Mem_print();
	
	// will remove 32 from a different block than the allocation before
	num_bytes_6 = (32)*unit_size;
        p6 = (int *) Mem_alloc(num_bytes_6);
        printf("first: %d bytes (%d units) at p=%p \n", 
                num_bytes_6, num_bytes_6/unit_size, p6);
        Mem_print();

	// free remaining memory	
	printf("second free of 112 units p=%p \n", p4);
        Mem_free(p4);
        Mem_print();

	printf("third free of 90 units p=%p \n", p1);
        Mem_free(p1);
        Mem_print();

	printf("forth free of 27 units p=%p \n", p3);
        Mem_free(p3);
        Mem_print();

        printf("fifth free of 30 units p=%p \n", p5);
        Mem_free(p5);
        Mem_print();
        
        printf("sixth free of 33 units p=%p \n", p6);
        Mem_free(p6);
        Mem_print();
        
        printf("unit driver 5 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 5 -----\n");
    }
    // test for performance in equilibrium 
    if (dprms.EquilibriumTest)
        equilibriumDriver(&dprms);

    exit(0);
}

/* ----- equilibriumDriver -----
 *
 * This is a driver to test the performance of the dynamic memory allocation
 * and free calls in equilibrium.  This code assumes that the functions are
 * defined in mem.h 
 *
 * The driver allocates dynamic memory for variable sized arrays of integers.
 * The size of an array is uniformly distributed in the range [avg-range,
 * avg+range].  The average size and range can be specified on the command
 * line.
 *
 * During a warmup phase, calls are made to allocate the integer arrays and
 * the arrays are stored in an unsorted list using the llist.c module.  
 *
 * During the equilibrium phase, the code randomly chooses to either allocate a
 * new array, or return one of the arrays stored in the list.  The events are
 * equally likely.  If an array is removed from the list and freed, one of the
 * list items is choosen with an equal probability over all items in the list.
 *
 * Finally, the last phase frees all arrays stored in the list.
 *
 * At the end of each phase, Mem_stats is called to print information about
 * the size of the free list.  In verbose mode, Mem_print is called to print
 * the address and size of each item in the free list.  Only enable verbose
 * mode when testing with small warmup and trial phases.
 *
 * The following parameters can be set on the command line.  If not set,
 * default values are used.
 *
 * -w 1000   number of warmup allocations
 * -t 100000 number of trials in equilibrium
 * -a 128    average size of interger array
 * -r 127    range for average size of interger array
 * -d        use system malloc/free instead of MP4 versions
 */
void equilibriumDriver(driver_params *ep)
{
    int i, index;
    int range_num_ints;
    int min_num_ints;
    int *ptr;
    int size;
    int pos;
    llist_t *mem_list;
    clock_t start, end;

    // print parameters for this test run 
    printf("\nEquilibrium test driver using ");
    if (ep->SysMalloc)
        printf("system malloc and free\n");
    else
        printf("Mem_alloc and Mem_free from mem.c\n");
    printf("  Trials in equilibrium: %d\n", ep->Trials);
    printf("  Warmup allocations: %d\n", ep->WarmUp);
    printf("  Average array size: %d\n", ep->AvgNumInts);
    printf("  Range for average array size: %d\n", ep->RangeInts);

    mem_list = llist_construct(NULL);
    // the size of the integer array is uniformly distributed in the range
    // [avg-range, avg+range]

    range_num_ints = 2 * ep->RangeInts + 1;
    min_num_ints = ep->AvgNumInts - ep->RangeInts;
    if (min_num_ints < 1 || ep->AvgNumInts < 1 || ep->RangeInts < 0) {
        printf("The average array size must be positive and greater than the range\n");
        exit(1);
    }

    // warmup by allocating memory and storing in list 
    for (i = 0; i < ep->WarmUp; i++) {
        // random size of array 
        size = ((int) (drand48() * range_num_ints)) + min_num_ints;
        if (ep->SysMalloc)
            ptr = (int *) malloc(size * sizeof(int));
        else
            ptr = (int *) Mem_alloc(size * sizeof(int));
        assert(ptr != NULL);
        // first position is size of array.  fill rest with numbers 
        ptr[0] = -size;
        for (index = 1; index < size; index++)
            ptr[index] = -index;   // same as *(ptr+index)=index 
        llist_insert(mem_list, (data_t *) ptr, LLPOSITION_BACK);
        ptr = NULL;
    }
    printf("After warmup\n");
    if (!ep->SysMalloc) {
        Mem_stats();
        if (ep->Verbose) Mem_print();
    } else {
        // OSX users: comment out next three lines
        struct mallinfo mi = mallinfo();
        printf("arena=%dB, chunks=%d, alloc=%d, free=%d\n", 
                mi.arena, mi.ordblks, mi.uordblks, mi.fordblks);
    }

    // in equilibrium make allocations and frees with equal probability 
    start = clock();
    for (i = 0; i < ep->Trials; i++) {
        if (drand48() < 0.5) {
            size = ((int) (drand48() * range_num_ints)) + min_num_ints;
            if (ep->Verbose) {
                // uncomment following print for more detail
                //printf("  list before allocation of size %d\n", size); 
                //Mem_print();
            }
            if (ep->SysMalloc)
                ptr = (int *) malloc(size * sizeof(int));
            else
                ptr = (int *) Mem_alloc(size * sizeof(int));
            assert(ptr != NULL);
            ptr[0] = -size;
            for (index = 1; index < size; index++)
                ptr[index] = -index;
            llist_insert(mem_list, (data_t *) ptr, LLPOSITION_BACK);
            ptr = NULL;
        } else if (llist_entries(mem_list) > 0) {
            pos = (int) (drand48() * llist_entries(mem_list));
            ptr = (int *) llist_remove(mem_list, pos);
            assert(ptr != NULL);
            size = -ptr[0];
            if (ep->Verbose) {
                // uncomment following print for more detail
                //printf("  list before freeing block with size %d from position %d\n", size, pos); 
                //Mem_print();
            }
            assert(min_num_ints <= size && size <= ep->AvgNumInts+ep->RangeInts);
            for (index = 1; index < size; index++)
                assert(ptr[index] == -index);
            if (ep->SysMalloc)
                free(ptr);
            else
                Mem_free(ptr);
            ptr = NULL;
        }
    }
    end = clock();
    printf("After exercise, time=%g\n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    if (!ep->SysMalloc) {
        Mem_stats();
        if (ep->Verbose) Mem_print();
    } else {
        // OSX users: comment out next three lines
        struct mallinfo mi = mallinfo();
        printf("arena=%dB, chunks=%d, alloc=%d, free=%d\n", mi.arena,
                mi.ordblks, mi.uordblks, mi.fordblks);
    }

    // remove and free all items from mem_list
    pos = llist_entries(mem_list);
    for (i = 0; i < pos; i++) {
        ptr = (int *) llist_remove(mem_list, LLPOSITION_FRONT);
        assert(ptr != NULL);
        size = -ptr[0];
        assert(min_num_ints <= size && size <= ep->AvgNumInts+ep->RangeInts);
        for (index = 1; index < size; index++)
            assert(ptr[index] == -index);
        if (ep->SysMalloc)
            free(ptr);
        else
            Mem_free(ptr);
        ptr = NULL;
    }
    assert(llist_entries(mem_list) == 0);
    llist_destruct(mem_list);

    printf("After cleanup\n");
    if (!ep->SysMalloc) {
        Mem_stats();
        if (ep->Verbose) Mem_print();
    } else {
        // OSX users: comment out next three lines
        struct mallinfo mi = mallinfo();
        printf("arena=%dB, chunks=%d, alloc=%d, free=%d\n", mi.arena,
                mi.ordblks, mi.uordblks, mi.fordblks);
    }
    printf("----- End of equilibrium test -----\n\n");
}


/* read in command line arguments.  Note that Coalescing and SearchPolicy 
 * are stored in global variables for easy access by other 
 * functions.
 *
 * -u 0  is for the unit drivers, starting with driver 0
 *
 * Other command line arguments are for the equilibrium driver parameters.
 */
void getCommandLine(int argc, char **argv, driver_params *ep)
{
    /* The geopt function creates three global variables:
     *    optopt--if an unknown option character is found
     *    optind--index of next element in argv 
     *    optarg--argument for option that requires argument 
     *
     * The third argument to getopt() specifies the possible argument flags
     *   If an argument flag is followed by a colon, then an argument is 
     *   expected. E.g., "x:y" means -x must have an argument but not -y
     */
    int c;
    int index;

    ep->Seed = 42657374;
    ep->Verbose = FALSE;
    ep->EquilibriumTest = FALSE;
    ep->WarmUp = 1000;
    ep->Trials = 100000;
    ep->AvgNumInts = 128;
    ep->RangeInts = 127;
    ep->SysMalloc = FALSE;
    ep->UnitDriver = -1;

    while ((c = getopt(argc, argv, "w:t:s:a:r:f:u:cdve")) != -1) {
        switch(c) {
            case 'u': ep->UnitDriver = atoi(optarg);   break;
            case 'w': ep->WarmUp = atoi(optarg);       break;
            case 't': ep->Trials = atoi(optarg);       break;
            case 's': ep->Seed = atoi(optarg);         break;
            case 'a': ep->AvgNumInts = atoi(optarg);   break;
            case 'r': ep->RangeInts = atoi(optarg);    break;
            case 'd': ep->SysMalloc = TRUE;            break;
            case 'v': ep->Verbose = TRUE;              break;
            case 'e': ep->EquilibriumTest = TRUE;      break;
            case 'c': Coalescing = TRUE;               break;
            case 'f':
                  if (strcmp(optarg, "best") == 0)
                      SearchPolicy = BEST_FIT;
                  else if (strcmp(optarg, "first") == 0)
                      SearchPolicy = FIRST_FIT;
                  else {
                      fprintf(stderr, "invalid search policy: %s\n", optarg);
                      exit(1);
                  }
                  break;
            case '?':
                  if (isprint(optopt))
                      fprintf(stderr, "Unknown option %c.\n", optopt);
                  else
                      fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                  printf("Lab4 command line options\n");
                  printf("General options ---------\n");
                  printf("  -v        turn on verbose prints (default off)\n");
                  printf("  -s 54321  seed for random number generator\n");
                  printf("  -c        turn on coalescing (default off)\n");
                  printf("  -f best|first\n");
                  printf("            search policy to find memory block (first by default)\n");
                  printf("  -u 0      run unit test driver\n");
                  printf("  -e        run equilibrium test driver\n");
                  printf("\nOptions for equilibrium test driver ---------\n");
                  printf("  -w 1000   number of warmup allocations\n");
                  printf("  -t 100000 number of trials in equilibrium\n");
                  printf("  -a 128    average size of interger array\n");
                  printf("  -r 127    range for average size of array\n");
                  printf("  -d        use system malloc/free instead of MP4 versions\n");
                  exit(1);
        }
    }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}

/* vi:set ts=8 sts=4 sw=4 et: */
