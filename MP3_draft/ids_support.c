/* ids_support.c
 * Cameron Burroughs
 * burrou5
 * ECE 2230 Fall 2020
 * MP2
 *
 * Propose: 	Use public functions from llist.h and ids_support.h 
 * 		to createsubroutines used for the handling of alerts
 * 		in two different types of doubly linked lists; sorted 
 * 		and unsorted.
 *
 * Assumptions: User will use sorted functions for sorted lists and 
 * 		unsorted functions for unsorted list.
 *
 * 		i.e. ids_scan is used only for sorted type lists
 *		
 *		Provided function protoypes, as seen in ids_support.h, 
 *		are pre-written and were not modified.
 *
 * Bugs:
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "llist.h"
#include "ids_support.h"

#define MAXLINE 256

// private functions for use in ids_support.c only
void ids_record_fill(alert_t *rec);   // collect input from user
void ids_print_alert_rec(alert_t *rec);  // print one record
int findSets(llist_t *list_ptr, int gen_id, int TH);  // set >= threshold?

void ids_sort_genid(llist_t *list_ptr, int sort_num)
{
    llist_sort(list_ptr, sort_num, ids_compare_genid);
    printf("In ids_: %d\n\n", list_ptr->ll_entry_count);
}

void ids_sort_destip(llist_t *list_ptr, int sort_num)
{
    llist_sort(list_ptr, sort_num, ids_match_destip);
}

void ids_append_rear(llist_t *list_ptr, int genid, int destip)
{
    alert_t *new_ptr = (alert_t *) calloc(1, sizeof(alert_t));
    new_ptr->dest_ip_addr = destip;
    new_ptr->generator_id = genid; 

    llist_insert(list_ptr, new_ptr, LLPOSITION_BACK);
    printf("Appended %d onto queue\n", new_ptr->dest_ip_addr);

}

/* ids_compare_genid is required by the list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal for sorting.
 *
 * For the alert data we want to sort from lowest ID up, so
 * closer to the front means a smaller ID.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ids_compare_genid(const alert_t *rec_a, const alert_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->generator_id < rec_b->generator_id)
	return 1;
    else if (rec_a->generator_id > rec_b->generator_id)
	return -1;
    else
	return 0;
}

/* ids_match_destip is used with llist_elem_find. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal for sorting.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ids_match_destip(const alert_t *rec_a, const alert_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->dest_ip_addr > rec_b->dest_ip_addr)
	return 1;
    else if(rec_a->dest_ip_addr < rec_b->dest_ip_addr)
    return -1;
    else
	return 0;
}

/* print one of the alert record lists
 *
 * inputs: 
 *    list_ptr: a pointer to either sorted or unsorted list
 *
 *    type_of_list: a charter string that must be either "List" or "Queue"
 *
 * output: prints the list in the format required for grading.
 *         Do not change any of these lines 
 */
void ids_print(llist_t *list_ptr, const char *list_type)
{
    assert(strcmp(list_type, "List")==0 || strcmp(list_type, "Queue")==0);
    int num_in_list = llist_entries(list_ptr);  // uses public llist.c function
    int index;
    if (num_in_list == 0) {
        printf("%s is empty\n", list_type);
    } else {
        printf("%s contains %d record%s\n", list_type, num_in_list,
                num_in_list==1 ? "." : "s.");
    }
    // prints each record
    for (index = 0; index < num_in_list; index++) {
        printf("%d: ", index+1);
       
	alert_t *rec_ptr = llist_access(list_ptr, index);   // uses public llist.c function
        ids_print_alert_rec(rec_ptr);
    }
    printf("\n");
}

/* This functions adds an alert record to the rear of a list.  The list is
 * unsorted, and it does not have any limits on the maximum size of the list.
 *
 * If the new alert has the same destination IP address as an alert that is
 * already in the list, the old matching alert is removed from the list and 
 * recycled.  The new alert is appended to the end of the list.
 *
 */
void ids_add_rear(llist_t *list_ptr)
{
    alert_t *new_ptr = (alert_t *) calloc(1, sizeof(alert_t));
    ids_record_fill(new_ptr);

    alert_t *match;
    int index = -2;

    match = llist_elem_find(list_ptr, new_ptr, &index, ids_match_destip);

    if(index == -1){  	// no match found
	// append to end
	llist_insert(list_ptr, new_ptr, LLPOSITION_BACK);
	printf("Appended %d onto queue\n", new_ptr->dest_ip_addr);
    } else { 	// match found 
	// remove old one at index
	match = llist_remove(list_ptr,index);
	free(match);
	match = NULL;

	// append to end
	llist_insert(list_ptr, new_ptr, LLPOSITION_BACK);
        printf("Appended %d onto queue and removed old copy\n", new_ptr->dest_ip_addr);
    }

}

/* This function removes the alert record at the front of the queue.  The
 * queue is unsorted, and there is no limit on the maximum size of the list.
 *
 * The alert memory block should be recycled so there are no memory leaks.
 */
void ids_remove_front(llist_t *list_ptr)
{
    alert_t *rec_ptr = NULL;
    rec_ptr = llist_remove(list_ptr, LLPOSITION_FRONT);
    
    if (rec_ptr != NULL) {
        printf("Deleted front with IP addr: %d\n", rec_ptr->dest_ip_addr);
    	free(rec_ptr); 	// recycle memory
	rec_ptr = NULL; // avoid dangling pointer
    } else {
        printf("Queue empty, did not remove\n");
    }
}

/* This creates a list and it can be either a sorted or unsorted list.
 * If the list is unsorted, a comparison function is not needed.  
 *
 * inputs: 
 *    list_type: a character string that must be either "List" or "Queue"
 *
 * This function is provided to you as an example of how to use a
 * function pointer.  
 *
 */
llist_t *ids_create(const char *list_type)
{
    assert(strcmp(list_type, "List")==0 || strcmp(list_type, "Queue")==0);
    if (strcmp(list_type, "List") == 0) {
        return llist_construct(ids_compare_genid); //sorted
    } else if (strcmp(list_type, "Queue") == 0) {
        return llist_construct(NULL); //unsorted
    } else {
        printf("ERROR, invalid list type %s\n", list_type);
        exit(1);
    }
}

/* This function adds an alert to the sorted list.  
 * If the new alert has the same generator ID as one or
 * more alerts in the list, then the new alert is placed after all the
 * existing alerts with the same address. 
 *
 * There is no return value since the insertion must always be 
 * successful, except in the catastrophic condition that the program
 * has run out of memory.
 *
 */

void ids_add(llist_t *list_ptr)
{
    alert_t *new_ptr = (alert_t *) calloc(1, sizeof(alert_t));
    ids_record_fill(new_ptr);

    llist_insert_sorted(list_ptr, new_ptr);  // adds alerts

    printf("Inserted %d into list\n", new_ptr->generator_id);
}

/* This function prints all alerts with the matching generator id in the
 * sorted list.  Print all matching alerts, and after the last print the 
 * number found.
 */
void ids_list_gen(llist_t *list_ptr, int gen_id)
{
    int count = 0;
    int max = llist_entries(list_ptr); 
    int i;

    alert_t* rover;

    for(i = 0;i < max;i++) {	//searches through alerts
	rover = llist_access(list_ptr, i);
	if(rover->generator_id == gen_id){
	    ids_print_alert_rec(rover);
	    count++;
	}
    }

    // prints summary
    if (count > 0)
        printf("Found %d alerts matching generator %d\n", count, gen_id);
    else
        printf("Did not find alert: %d\n", gen_id);

    rover = NULL; // avoids dangling pointer
}

/* This function prints all alerts with the matching destination ip_address in the
 * sorted list.  Print all matching alerts, and after the last print the 
 * number found.
 */
void ids_list_ip(llist_t *list_ptr, int dest_ip)
{
    int count = 0;
    int max = llist_entries(list_ptr); 
    int i;

    alert_t* rover;

    for(i = 0;i < max;i++) {	//searches through alerts
	rover = llist_access(list_ptr, i);
	if(rover->dest_ip_addr == dest_ip){
	    ids_print_alert_rec(rover);
	    count++;
	}
    }
    
    // prints summary
    if (count > 0)
        printf("Found %d alerts matching IP %d\n", count, dest_ip);
    else
        printf("Did not find destination IP: %d\n", dest_ip);

    rover = NULL; // avoids dangling pointer
}

/* This function removes all alerts from the sorted list with the matching
 * generator id.  Do not print the alerts that are removed.  
 */
void ids_remove_gen(llist_t *list_ptr, int gen_id)
{
    int count = 0;
    int max = llist_entries(list_ptr); 
    int i;

    alert_t* rover;

    for(i = 0;i < max; i++) {  // searches list
	rover = llist_access(list_ptr, i);
	if(rover->generator_id == gen_id) {
	    rover = llist_remove(list_ptr,i);
	    free(rover);       // avoids memory leak
	    count++;
	    i--;
	    max--; // resets size
	}
    }

    //prints summary
    if (count > 0)
        printf("Removed %d alerts matching generator %d\n", count, gen_id);
    else
        printf("Did not remove alert with generator: %d\n", gen_id);

    rover = NULL; // avoids dangling pointer
}

/* This function removes all alerts from the sorted list with the matching
 * generator id.  Do not print the alerts that are removed.  
 */
void ids_remove_ip(llist_t *list_ptr, int dest_ip)
{
    int count = 0;
    int max = llist_entries(list_ptr); 
    int i;

    alert_t* rover;

    for(i = 0;i < max; i++) {  // searches list
	rover = llist_access(list_ptr, i);
	if(rover->dest_ip_addr == dest_ip) {
	    rover = llist_remove(list_ptr,i);
	    free(rover);       // avoids memory leak
	    count++;
	    i--;
	    max--; // resets size
	}
    }

    //prints summary
    if (count > 0)
        printf("Removed %d alerts matching IP %d\n", count, dest_ip);
    else
        printf("Did not remove alert with IP: %d\n", dest_ip);
    
    rover = NULL; // avoids dangling pointer
}

/* This function is only used for a sorted list. It uses private function 
 * findSets to print out the details of all sets in list_ptr with more 
 * members than the given 'thresh' parameter.
 */
void ids_scan(llist_t *list_ptr, int thresh)
{
    int sets = 0;
    int minID, maxID, i;

    alert_t* tmp;

    tmp = llist_access(list_ptr, LLPOSITION_FRONT);
    minID = tmp->generator_id;

    tmp = llist_access(list_ptr, LLPOSITION_BACK);
    maxID = tmp->generator_id;

    tmp = NULL; // avoids dangling pointer

    for(i = minID; i <= maxID; i++) { // searches for eligible groups
	if(findSets(list_ptr, i, thresh)) { sets++;}
    }
    
    // after all sets have been discovered print one of the following
    if (sets > 0) 
        printf("Scan found %d sets\n", sets);
    else
        printf("Scan found no alerts with >= %d matches\n", thresh);
}

/* This function is a private function for use in ids_support.c only
 * It counts the number of members in a set using the gen_id. 
 * 
 * Return values:
 * 		1    if the # of members in the set is >= threshold
 * 		0    if the # of members in the set is < threshold
 */
int findSets(llist_t *list_ptr, int gen_id, int TH)
{
    int i; 
    int count = 0;

    int max = llist_entries(list_ptr);

    alert_t* rover;
    for(i = 0; i < max; i++) {
	rover = llist_access(list_ptr, i);
	if(rover->generator_id == gen_id)
	    count++;
    }
    rover = NULL; // avoids dangling pointer
    
    if(count >= TH && count != 0) {
	// for each set that is found print the alert summary
	printf("A set with generator %d has %d alerts\n", gen_id, count);
	return 1;
    } else { return 0;}
}

/* the function takes a pointer to each list and prints the
 * number of items in each list
 */
void ids_stats(llist_t *sorted, llist_t *unsorted)
{
    // get the number in list and size of the list
    int num_in_sorted_list = llist_entries(sorted);
    int num_in_unsorted_list = llist_entries(unsorted);
    printf("Number records in list: %d, queue size: %d\n", 
            num_in_sorted_list, num_in_unsorted_list);
}

/* this function frees the memory for either a sorted or unsorted list.*/
void ids_cleanup(llist_t *list_ptr)
{
    llist_destruct(list_ptr);  // frees memory to avoid leaks
}

/* Prompts user for alert record input starting with the generator ID.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * The input to the function assumes that the structure has already been
 * created.  The contents of the structure are filled in.
 *
 * There is no output.
 *
 * Do not change the sequence of prompts as grading requires this exact
 * format
 */
void ids_record_fill(alert_t *rec)
{
    char line[MAXLINE];
    assert(rec != NULL);

    printf("Generator component:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->generator_id);
    printf("Signature:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->signature_id);
    printf("Revision:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->revision_id);
    printf("Dest IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->dest_ip_addr);
    printf("Source IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->src_ip_addr);
    printf("Destination port number:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->dest_port_num);
    printf("Source port number:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->src_port_num);
    printf("Time:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &rec->timestamp);
    printf("\n");
}

/* print the information for a particular alert record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * Do not change any of these lines and grading depends on this
 * format.
 */
void ids_print_alert_rec(alert_t *rec)
{
    assert(rec != NULL);
    printf("[%d:%d:%d] (gen, sig, rev): ", rec->generator_id, rec->signature_id, 
            rec->revision_id);
    printf("Dest IP: %d, Src: %d, Dest port: %d,", rec->dest_ip_addr, 
            rec->src_ip_addr, rec->dest_port_num);
    printf(" Src: %d, Time: %d\n", rec->src_port_num, rec->timestamp); 
}

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
