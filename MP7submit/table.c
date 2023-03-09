/* table.c
 * Cameron L. Burroughs
 * burrou5
 * C17134631
 * MP7 
 * ECE 2230 Fall 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "table.h"

#define MAX(x,y) ((x > y) ? x : y) 

//private functions
unsigned int hash1(hashkey_t K);
unsigned int hash2(hashkey_t K);
void table_free_ll(sep_chain_t* L);

/*  The empty table is created.  The table must be dynamically allocated and
 *  have a total size of table_size.  The maximum number of (K, I) entries
 *  that can be stored in the table is table_size-1.  For open addressing, 
 *  the table is filled with a special empty key distinct from all other 
 *  nonempty keys (e.g., NULL).  
 *
 *  the probe_type must be one of {LINEAR, DOUBLE, CHAIN}
 *
 *  Do not "correct" the table_size or probe decrement if there is a chance
 *  that the combinaion of table size or probe decrement will not cover
 *  all entries in the table.  Instead we will experiment to determine under
 *  what conditions an incorrect choice of table size and probe decrement
 *  results in poor performance.
 */
table_t *table_construct(int table_size, int probe_type)
{
    assert(table_size > 1);
    assert(probe_type >= 0 && probe_type <= 2);

    table_t* T = (table_t*)calloc(sizeof(table_t), 1);
    if(probe_type != CHAIN) {
	T->oa = (table_entry_t*)calloc(sizeof(table_entry_t),table_size);
	for(int i = 0; i < T->size; i++){
	    T->oa[i].key = NULL;
	    T->oa[i].deleted = 0;
	    T->oa[i].data_ptr = NULL;
	}
	T->sc = NULL;
    } else {
	T->sc = (sep_chain_t**)calloc(sizeof(sep_chain_t*),table_size);
	for(int i = 0; i < T->size; i++){
	    T->sc[i] = NULL;
	} 
	T->oa = NULL;
    }
    T->size = table_size;
    T->num_keys = 0;
    T->recent_probes = 0;
    T->probe_t = probe_type;

    return T;
} 

/* Sequentially remove each table entry (K, I) and insert into a new
 * empty table with size new_table_size.  Free the memory for the old table
 * and return the pointer to the new table.  The probe type should remain
 * the same.
 *
 * Do not rehash the table during an insert or delete function call.  Instead
 * use drivers to verify under what conditions rehashing is required, and
 * call the rehash function in the driver to show how the performance
 * can be improved.
 */
table_t *table_rehash(table_t * T, int new_table_size)
{
    table_t* new;
        
    if(T == NULL) return NULL;
    
    new = table_construct(new_table_size, T->probe_t);
    if(T->probe_t != CHAIN) {
	for(int i = 0; i < T->size; i++) {
	    table_insert(new, T->oa[i].key, T->oa[i].data_ptr);
	}
	free(T->oa);
	free(T);
    } else {
	sep_chain_t *rover, *temp;
	for(int i = 0; i < T->size; i++) {
	    rover = T->sc[i];
	    while(rover != NULL) {
		table_insert(new,rover->key, rover->data_ptr);
		temp = rover;
		rover = rover->next;
		free(temp);
	    }
	}
	free(T->sc);
	free(T);
    }
    return new;
} 

/* returns number of entries in the table */
int table_entries(table_t *T)
{
    return T->num_keys;
}

/* returns 1 if table is full and 0 if not full. 
 * For separate chaining the table is never full
 */
int table_full(table_t *T)
{
    if(T->probe_t == CHAIN)
	return 0;

    if((T->size - 1) == T->num_keys) 
	return 1;
    else 
	return 0;
}

/* returns the number of table entries marked as deleted */
int table_deletekeys(table_t *T)
{
    if(T->probe_t == CHAIN) 
	return 0;

    int del = 0;
    for(int i = 0; i < T->size; i++) {
        if(T->oa[i].deleted)
	    del++;
    }
    return del; 
}
   
/* Insert a new table entry (K, I) into the table provided the table is not
 * already full.  
 * Return:
 *      0 if (K, I) is inserted, 
 *      1 if an older (K, I) is already in the table (update with the new I), or 
 *     -1 if the (K, I) pair cannot be inserted.
 *
 * Note that both K and I are pointers to memory blocks created by malloc()
 */
int table_insert(table_t *T, hashkey_t K, data_t I)
{
    if(K == NULL || T == NULL) { 
	return -1;
    }

    int full = table_full(T);
    int index = hash1(K)%T->size;  // h(K)

    if(T->probe_t == LINEAR || T->probe_t == DOUBLE) {
	unsigned int h2;
	int start = index;
	int decrement; 
	int deleted_index = -1;
	int probe = 1;
	int flag = 0;
	

	if(T->probe_t == LINEAR) {
	    decrement = 1;
	} else {
	    h2 = hash2(K);
	    decrement = MAX(1, h2 % T->size);  // p(K)
	}

//	printf("Key: %7s\tIndex: %d\tDecrement: %d\n", K, index,  decrement); // this is for testing purposes

	/* CONDITIONS TO EXIT WHILE LOOP:
	 *    1) if an entry with key K is already present
	 *    2) if the entire table has been searched
	 *    3) if a non-deleted empty key is found
	*/
	flag = flag || (T->oa[index].key != NULL && (strcmp(T->oa[index].key, K) == 0));
	flag = flag || (T->oa[index].key == NULL && !T->oa[index].deleted);
        while(!flag) {
	    probe++;
	    // if deleted entry is found, save index, keep searching
	    if(T->oa[index].deleted && deleted_index == -1) {
		deleted_index = index;
	    }
	    index -= decrement;
	    if(index < 0) {
		index = index + T->size;
	    }

	    // recheck conditions
	    flag = flag || (T->oa[index].key != NULL && (strcmp(T->oa[index].key, K) == 0));
	    flag = flag || (index == start);
	    flag = flag || (T->oa[index].key == NULL && !T->oa[index].deleted);
	}
	
	// if matching key found, update pointer
        if(T->oa[index].key != NULL && strcmp(T->oa[index].key, K) == 0) {
	    free(T->oa[index].data_ptr);
	    T->oa[index].data_ptr = I;
	    T->recent_probes = probe;
	    free(K);
	    return 1;
	}
	// if deleted found before empty, insert at deleted
	if(deleted_index != -1) {
	    T->oa[deleted_index].key = K;
	    T->oa[deleted_index].data_ptr = I;
	    T->oa[deleted_index].deleted = 0;
	    T->recent_probes = probe;
	    T->num_keys++;
	    return 0;
	}
	// if empty entry found before deleted, insert at empty
	if(T->oa[index].key == NULL && !full) {
	    T->oa[index].key = K;
	    T->oa[index].data_ptr = I;
	    T->recent_probes = probe;
	    T->num_keys++;
	    return 0;
	}
	free(I);
	return -1; // could not be inserted

    } else {  // SEPERATE CHAINING
	int probe = 1;

//	printf("Key: %15s\tIndex: %d\n", K, index); // this is for testing purposes

	// if no entry exists at index
	if(T->sc[index] == NULL) {
	    T->sc[index] = (sep_chain_t*)malloc(sizeof(sep_chain_t));
	    T->sc[index]->key = K;
	    T->sc[index]->data_ptr = I;
	    T->sc[index]->next = NULL;
	    T->num_keys++;
	    T->recent_probes = probe;
	    return 0;
	}
	// go through linked list til a common key is found, or you've 
	// reached the end of the list
	sep_chain_t* rover = T->sc[index];
	while(rover->next && (strcmp(rover->key,K) != 0)) {
	    rover = rover->next;
	    probe++;
	}
	// if matching found, update components
	if(rover->key != NULL && strcmp(rover->key, K) == 0) {
	    free(rover->data_ptr);
	    rover->data_ptr = I;
	    T->recent_probes = probe;
	    free(K);
	    return 1;
	}
	// if no matching key found
	if(rover->next == NULL) {
	    rover->next = (sep_chain_t*)malloc(sizeof(sep_chain_t));
	    rover->next->key = K;
	    rover->next->data_ptr = I;
	    rover->next->next = NULL;
	    T->recent_probes = probe;
	    T->num_keys++;
	    return 0;
	}

	free(I);
	return -1;
    }

}


// bernstein hash algorithm from eternallyconfuzzled.com
unsigned int hash1(hashkey_t K)
{
    unsigned char *p = (unsigned char*)K;
    unsigned int h = 0;
    int len = strlen(K);

    for(int i = 0; i < len; i++) {
	h = 33 * h + p[i];
    }
    return h;
}


// modified bernstein hash algorithm from eternallyconfuzzled.com
unsigned int hash2(hashkey_t K)
{
    unsigned char *p = (unsigned char*)K;
    unsigned int h = 0;
    int len = strlen(K);

    for(int i = 0; i < len; i++) {
	h = 33 * h ^ p[i];
    }
    return h;
}


/* Delete the table entry (K, I) from the table.  Free the key in the table.
 * Return:
 *     pointer to I, or
 *     null if (K, I) is not found in the table.  
 *
 * Be sure to free(K)
 *
 * See the note on page 490 in Standish's book about marking table entries for
 * deletions when using open addressing.
 */
data_t table_delete(table_t *T, hashkey_t K)
{
    data_t I = NULL;
    int index = hash1(K)%T->size;
    int decrement;
    int flag = 1;
    int probe = 0;

//    printf("Key: %15s\tIndex: %d\n", K, index); // this is for testing purposes
    if(T->probe_t == CHAIN) {
	probe++;
	sep_chain_t *rover, *prev;
	rover = T->sc[index];
	// nothing found at index
	if(rover == NULL) {
	    T->recent_probes = probe;
	    return NULL;
	}
	// first link has matching key
	if(strcmp(rover->key, K) == 0) {
	    T->sc[index] = rover->next;
	    I = rover->data_ptr; 
	    free(rover->key);
	    free(rover);
	    T->num_keys--;
	// probe linked list for key
	} else {
	    prev = rover;
	    rover = rover->next;
	    while(rover != NULL) {
		probe++;
		if(strcmp(rover->key, K) == 0) {
		    prev->next = rover->next;
		    I = rover->data_ptr;
		    free(rover->key);
		    free(rover);
		    rover = NULL;
		    T->num_keys--;
		}
		prev = rover;
		rover = rover->next;
	    }
	}
    } else {
	int start = index;
	// determine decrement
	if(T->probe_t == LINEAR) 
	    decrement = 1;
	else 
	    decrement = MAX(1,hash2(K)%T->size);
	//search for key
	while(flag) {
	    probe++;
	    // key found, free components, mark as empty & deleted
	    if(T->oa[index].key != NULL && strcmp(T->oa[index].key,K) == 0) {
		I = T->oa[index].data_ptr;
		free(T->oa[index].key);
		T->oa[index].key = NULL;
		T->oa[index].deleted = 1;
		T->num_keys--;
		flag = 0;
	    // empty, non deleted key found, exit while loop
	    } else if(T->oa[index].key == NULL && !T->oa[index].deleted) {
		flag = 0;
	    }
	    index -= decrement;
	    if(index < 0)
		index += T->size;
	    if(index == start)
		flag = 0;
	}
    } 
    T->recent_probes = probe;    
    return I;
}

/* Given a key, K, retrieve the pointer to the information, I, from the table,
 * but do not remove (K, I) from the table.  Return NULL if the key is not
 * found.
 */
data_t table_retrieve(table_t *T, hashkey_t K)
{
    data_t I = NULL;
    int index = hash1(K)%T->size;
    int decrement;
    int flag = 1;
    int probe = 0;

    if(T->probe_t == CHAIN) {
	probe++;
	sep_chain_t* rover;
	rover = T->sc[index];
	// search linked list for key
	while(rover != NULL) {
	    probe++;
	    if(strcmp(rover->key, K) == 0) {
		I = rover->data_ptr;
		rover = NULL;
	    } else {
		rover = rover->next;
	    }
	}
    } else {
	int start = index;

	// determine decrement
	if(T->probe_t == LINEAR) 
	    decrement = 1;
	else 
	    decrement = MAX(1,hash2(K)%T->size);

	while(flag) {
	    probe++;
	    // key found, set I to point to data, exit loop
	    if(T->oa[index].key != NULL && strcmp(T->oa[index].key,K) == 0) {
		I = T->oa[index].data_ptr;
		flag = 0;
	    // empty, non-deleted key found, exit loop
	    } else if(T->oa[index].key == NULL && !T->oa[index].deleted) {
		flag = 0;
	    }
	    index -= decrement;
	    if(index < 0)
		index += T->size;
	    if(index == start)
		flag = 0;
	}
    } 

    T->recent_probes = probe;
    return I;
}

/* Free all information in the table, the table itself, and any additional
 * headers or other supporting data structures.  
 */
void table_destruct(table_t *T)
{
    int i = 0;

    if(T->probe_t == LINEAR || T->probe_t == DOUBLE) {
	for( ; i < T->size; i++) {
	    if(T->oa[i].key != NULL)
		free(T->oa[i].data_ptr);
		free(T->oa[i].key);
	}
	free(T->oa);
    } else {
	for( ; i < T->size; i++) {
	    if(T->sc[i] != NULL)
		table_free_ll(T->sc[i]);
	}
	free(T->sc);
    }
    free(T);
}

// recursive helper function to free all nodes in a linked list
void table_free_ll(sep_chain_t* L)
{
    if(L->next != NULL)
	table_free_ll(L->next);

    free(L->data_ptr);
    free(L->key);
    free(L);
}

/* The number of probes for the most recent call to table_retrieve,
 * table_insert, or table_delete 
 */
int table_stats(table_t *T)
{
    return T->recent_probes;
}

/* This function is for testing purposes only.  Given an index position into
 * the hash table return the value of the key if data is stored in this 
 * index position.  If the index position does not contain data, then the
 * return value must be zero.  Make the first
 * lines of this function 
 *       assert(0 <= index && index < table_size); 
 *       assert(0 <= list_position); 
 */
hashkey_t table_peek(table_t *T, int index, int list_position)
{
    assert(0 <= index && index < T->size);
    assert(0 <= list_position);

    if(list_position > T->num_keys)
	return 0;


    if(T->probe_t != CHAIN) {
	if(T->oa[index].key == NULL)
	    return 0;
	else 
	    return T->oa[index].key;
    } else {
	if(T->sc[index] == NULL)
	    return 0;
	int count = 0;
	sep_chain_t *rover = T->sc[index];
	while(count < list_position && rover != NULL) {
	    count++;
	    rover = rover->next;
	}
	if(rover == NULL)
	    return 0;
	else 
	    return rover->key;
    }	
}

/* Print the table position and keys in a easily readable and compact format.
 * Only useful when the table is small.
 */
void table_debug_print(table_t *T)
{
    assert(T != NULL);

    int i = 0;
    int PT = T->probe_t;
    
    if(PT != CHAIN) {
	for( ; i < T->size; i++) {
	    if(T->oa[i].key == NULL)
		if(T->oa[i].deleted)
		    printf("%d: [  del  ]\n", i);
		else
		    printf("%d: \n", i);
	    else
		printf("%d: %s\n", i, T->oa[i].key);
	}
	printf("\n");
    } else {
	sep_chain_t* rover;
	for( ; i < T->size; i++) {
	    if(T->sc[i] == NULL) 
		printf("%d: -\n", i);
	    else {
		rover = T->sc[i];
		printf("%d:", i);
		while(rover != NULL) {
		    printf(" -- %s",rover->key);
		    rover = rover->next;
		}
		printf("\n");
	    }
	}
	printf("\n");
    }
}
