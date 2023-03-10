/* lab2.c
 * Cameron Burroughs
 * burrou5
 * ECE 2230 Fall 2020
 * MP2
 *
 * Purpose: 	Controls user interface for list and queue handling 
 *
 * Assumptions: The main function simply collects input commands and
 *              calls the appropriate ids function.
 *		
 *		User will input correct values or they will be defaulted
 *		to an acceptable value.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "llist.h"
#include "ids_support.h"

#define MAXLINE 256

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    char command[MAXLINE];
    char junk[MAXLINE];
    int command_number_1;   // number found with command
    int command_number_2;
    int num_items;
    llist_t *unsorted_list = NULL;  // Pointer to unsorted list
    llist_t *sorted_list = NULL;  // Pointer to sorted list

    if (argc != 1) {
        printf("Usage: ./lab2 \n");
        exit(1);
    }
    printf("Welcome to lab2.\n");
    printf("Sorted list: ALERT; LISTGEN x; LISTIP x; DELGEN x; DELIP x; SCANGEN x; PRINT\n");
    printf("Queue      : ADDREAR; RMFRONT; PRINTQ; SORTGEN x; SORTIP x; APPENDREAR;\n");
    printf("           : STATS; QUIT\n");

    // this list is sorted 
    sorted_list = ids_create("List");

    // this list is unsorted and has no duplicates
    unsorted_list = ids_create("Queue");

    // remember fgets includes newline \n unless line too long 
    while (fgets(line, MAXLINE, stdin) != NULL) {
        num_items = sscanf(line, "%s%d%d%s", command, &command_number_1, &command_number_2, junk);
        if (num_items == 1 && strcmp(command, "ALERT") == 0) {
            ids_add(sorted_list);
        } else if (num_items == 2 && strcmp(command, "LISTGEN") == 0) {
            ids_list_gen(sorted_list, command_number_1);
        } else if (num_items == 2 && strcmp(command, "LISTIP") == 0) {
            ids_list_ip(sorted_list, command_number_1);
        } else if (num_items == 2 && strcmp(command, "DELGEN") == 0) {
            ids_remove_gen(sorted_list, command_number_1);
        } else if (num_items == 2 && strcmp(command, "DELIP") == 0) {
            ids_remove_ip(sorted_list, command_number_1);
        } else if (num_items == 2 && strcmp(command, "SCANGEN") == 0) {
            ids_scan(sorted_list, command_number_1);
        } else if (num_items == 1 && strcmp(command, "PRINT") == 0) {
            ids_print(sorted_list, "List");
        } else if (num_items == 1 && strcmp(command, "ADDREAR") == 0) {
            ids_add_rear(unsorted_list);
        } else if (num_items == 1 && strcmp(command, "RMFRONT") == 0) {
            ids_remove_front(unsorted_list);
        } else if (num_items == 1 && strcmp(command, "PRINTQ") == 0) {
            ids_print(unsorted_list, "Queue");
        } else if (num_items == 1 && strcmp(command, "STATS") == 0) {
            ids_stats(sorted_list, unsorted_list);
        } else if (num_items == 2 && strcmp(command, "SORTGEN") == 0) {
            ids_sort_genid(unsorted_list, command_number_1);
        } else if (num_items == 2 && strcmp(command, "SORTIP") == 0) {
            ids_sort_destip(unsorted_list, command_number_1);
        } else if (num_items == 3 && strcmp(command, "APPENDREAR") == 0) {
            ids_append_rear(unsorted_list, command_number_1, command_number_2);
        } else if (num_items == 1 && strcmp(command, "QUIT") == 0) {
            ids_cleanup(sorted_list);
            ids_cleanup(unsorted_list);
            printf("Goodbye\n");
            break;
        } else {
            printf("# %s", line);
        }
    }
    exit(0);
}

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
