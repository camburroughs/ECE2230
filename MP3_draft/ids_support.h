/* ids_support.h
 * Cameron Burroughs
 * burrou5
 * ECE 2230 Fall 2020
 * MP2
 *
 * Propose: A header file for ids_support.c
 *
 * Assumptions: All function prototypes in this file start with 'ids_' and are
 * 		public functions.
 *
 * 		Prototypes for these functions have not been changed from 
 * 		original starter file.
 * Bugs: N/A
 *
 */

/* prototype function definitions */

/* function to compare alert records */
int ids_compare_genid(const alert_t *soc_a, const alert_t *soc_b);
int ids_match_destip(const alert_t *rec_a, const alert_t *rec_b);

/* functions to create and cleanup alert lists */
llist_t *ids_create(const char *);
void ids_cleanup(llist_t *);

/* Functions to get and print alert information */
void ids_print(llist_t *list_ptr, const char *); /* print list of records */
void ids_stats(llist_t *, llist_t *);            /* prints size of each list */

/* functions for sorted list */
void ids_add(llist_t *);
void ids_list_gen(llist_t *, int);
void ids_list_ip(llist_t *, int);
void ids_remove_gen(llist_t *, int);
void ids_remove_ip(llist_t *, int);
void ids_scan(llist_t *, int);

/* functions for unsorted list to insert at the rear and remove at the front.
 * Duplicate alerts with the same destination IP address are not
 * permitted.  If a new alert is inserted the old one must be
 * recycled
 */
void ids_add_rear(llist_t *);
void ids_remove_front(llist_t *);

/* New functions for MP3 */
void ids_sort_genid(llist_t *list_ptr, int sort_num);
void ids_sort_destip(llist_t *list_ptr, int sort_num);
void ids_append_rear(llist_t *list_ptr, int genid, int destip);

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
