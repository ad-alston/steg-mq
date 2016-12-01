/**
 * mq.c
 *
 * Main driver for steg-mq.  Listens on the UNIX domain
 * interface and responds to requests accordingly.
 *
 * Aubrey Alston (ada2145@columbia.edu)
 */

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stream_interface.h"
#include "queue.h"
#include "commands.h"

struct queue* message_queue;

/**
 * Prints a message to stderr.
 */
void print_error(char* msg){
	fprintf(stderr, "Steg-MQ ERROR: %s\n", msg);
}

/**
 * Encrypts an integer value using AES-256 in ECB 
 * mode.
 */
void encrypt_counter(int value, char* key){
	
}

/**
 * Reads a message and a key from the client stream, 
 * applies a PRP (AES-CTR), and publishes the stegotext
 * to the queue.
 */
void do_publish_stegotext(int client_sock){
	// Get message length
	int messageLength;
	if( readBytes(client_sock, sizeof(messageLength), 
			(void*) (&messageLength)) < 0 ){
		print_error("Error while reading from client stream.");
	}

	// Get message	
	char* data;
	data = malloc(messageLength);
	if( readBytes(client_sock, messageLength, (void*) data) < 0 ){
		print_error("Error while reading message from client stream.");
	}

	// Get key
	char key[16];
	if( readBytes(client_sock, 16, (void*) key) ){
		print_error("Error while reading key from client stream.");
	}

	int i;
	for( i = 0; i <= messageLength/16; i++ ){
	
	}


	free(data);
}

/**
 * Serves a client specified by the client socket
 * corresponding to his or her connection.
 */
void serve(int client_sock){
	char command[1];
	if(read(client_sock, command, 1) < 1){
		// Nothing sent.  Return.
		return;
	}

	switch(command[0]){
		case CMD_PUBLISH_STXT:
			do_publish_stegotext(client_sock);
		default:
			print_error("Unknown command received.");
	}
}


int main(int argc, char** argv){
	int intf_sock;

	// For now, just create a single queue used by everybody
	message_queue = new_queue();

	// Start the interface
	if( (intf_sock = interface_start()) < 0){
		print_error("Unable to bind to UNIX domain socket.");
		return -1;
	}	

	// Accept and serve connections
	while(1){
		int client;		
		if( (client = accept(intf_sock, NULL, NULL)) < 0){
			print_error("Error while accepting client connection.");
		}
		serve(client);
	}

	return 0;
}
