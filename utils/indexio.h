#pragma once
/* 
 * indexio.h --- save and load an index to a file indexnm
 * 
 * Author: Eric Chen
 * Created: Tues Oct 27
 * Version: 1.0
 * 
 * Description: 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "queue.h"
#include "hash.h"

#define HTABLE_SIZE 128

/* object stored in queue that hold doc id and wc within that doc */
typedef struct counters {
	int id;
	uint32_t count;
} counters_t;

/* object to store each unique word for the hashtable and associated queue*/
typedef struct wc {
	char *word;
	queue_t *qp;
} wc_t;

/*
 * make a wc_t object
 *   - malloc space for object and for word
 *   - used to make a wordcount to place into a hash table
 */
wc_t* make_wc(char *word, counters_t *qp);


/*
 * make a counters_t object
 *   - malloc space for doc
 *   - used to make a document count to place into a queue
 */
counters_t* make_doc(int id, uint32_t count);


/*
 * free all wc_t objects in hash table before calling hclose()
 */
void freeWord(void* elementp);


/*
 * indexsave
 */
void indexsave(hashtable_t *hp, char *indexnm);

/* 
 * indexload
 */
//webpage_t *indexload(int id, char *dirnm);
