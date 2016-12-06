/**
 * Simple queue implementation
 *
 * Aubrey Alston (ada2145@columbia.edu)
 */

#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void queue_push(struct queue* q, struct message* m){
	if(q->size == 0){
		q->head = m; q->tail = m;
	} else{
		q->tail->next = m;
		m->prev = q->tail;
		q->tail = m;
	}
	m->next = (struct message*) 0;
	q->size++;
	q->counter++;
}

struct message* queue_pop(struct queue* q){
	struct message* ret;
	if(q->size == 0){
		return (struct message*) 0;
	} else{
		ret = q->head;
		if(ret->next != (struct message*) 0)
			ret->next->prev = (struct message*) 0;
		else
			q->tail = (struct message*) 0;
		q->head = ret->next;
	}
	q->size--;
	return ret;
}

struct queue* new_queue(void){
	struct queue* q;
	
	q = malloc(sizeof(struct queue));
	
	q->counter = 0;
	q->size = 0;

	q->head = (struct message*) 0;
	q->tail = (struct message*) 0;

	return q;
}

struct message* new_message(char* data, int len){
	struct message* m;

	m = malloc(sizeof(struct message));
	m->next = (struct message*) 0;
	m->prev = (struct message*) 0;
	
	m->data = malloc(len);
	m->len = len;

	memcpy(m->data, data, len);

	return m;
}
