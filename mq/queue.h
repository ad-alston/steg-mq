/**
 * Struct definitions and function declarations for 
 * functionalities required by steg-mq queues.
 *
 * Aubrey Alston (ada2145@columbia.edu)
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__

struct message {
	struct message* next;
	struct message* prev;
	char* data;
	int len;
};

struct queue {
	// Synchronized message counter
	int counter;

	// Tracks the size of the queue
	int size;
	struct message* head;
	struct message* tail;
};

void queue_push(struct queue* q, struct message* msg);
struct message* queue_pop(struct queue* q);

struct queue* new_queue(void);
struct message* new_message(char* data, int len);

#endif
