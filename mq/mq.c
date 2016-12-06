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
 * Prints a block in hexadecimal format.
 */
void print_block(char* msg, unsigned char* block, int byte_len){
	fprintf(stderr, "%s\n", msg);

	int j; 
	for(j = 0; j < byte_len; j++) fprintf(stderr, "%02x ", block[j]);
	fprintf(stderr, "\n");

}

/**
 * Encrypts an integer value using AES-256 in ECB 
 * mode.
 */
int encrypt_counter(int value, unsigned char* key, unsigned char* out){
	unsigned char data[16];
	unsigned char iv[16]; // placeholder IV for counter

	// Copy data to 128-bit buffer
	memset((void*) data, 0, 16);
	memset((void*) iv, 0, 16);
	memcpy((void*) data, (void*) &value, sizeof(int));

	// Initialize EVP cipher context
	EVP_CIPHER_CTX *ctx;
	if(! (ctx = EVP_CIPHER_CTX_new()) ) return -1;  	

	if(! EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, iv)){
		return -1;
	}
	
	// Encrypt the block
	int len;
	if(1 != EVP_EncryptUpdate(ctx, out, &len, data, 16)){
		return -1;
	}

	EVP_CIPHER_CTX_free(ctx);

	return 0;
}

/**
 * Reads a message and a key from the client stream, 
 * applies a PRP (AES-CTR), and publishes the stegotext
 * to the queue.
 */
void do_publish_stegotext(int client_sock){	
	// Get message length
	int messageLength;
	if( read(client_sock, (void*) &messageLength, sizeof(messageLength)) < 4){
		print_error("Error while reading from client stream.");
		return;
	}
	// Get message	
	char* data;
	data = malloc(messageLength);
	if( read(client_sock, (void*) data, messageLength) < messageLength ){
		print_error("Error while reading message from client stream.");
		return;
	}

	// Get key; need to verify key length
	unsigned char key[32];
	int res;
	if( (res = read(client_sock, (void*) key, 32)) < 32 ){
		fprintf(stderr, "%d\n", res);
		print_error("Error while reading key from client stream.");
		return;
	}

	int i;
	for( i = 0; i <= messageLength/16; i++ ){
		// Retrieve mask (pseudorandom)
		char block[16];
		encrypt_counter(message_queue->counter, key, block);
		
		// Mask message block
		int j;
		for(j = 0; j < 16 && (i*16 + j) < messageLength; j++){
			block[j] = block[j] ^ data[i*16 + j];
		}
	
		// Add the hidden message to the queue
		struct message* message;
		message = new_message(block, 16);
		queue_push(message_queue, message);

		print_block("DEBUG: stegotext block added to main queue: ", 
			block, 16);
	}

	free(data);
}

/**
 * Pops the top-most message from the stegotext queue 
 * and returns it to the client.
 */
void do_consume_stegotext(int client_sock){
	// Serve the top-most stegotext in the message queue
	struct message* message;
	message = queue_pop(message_queue);
	
	// If the queue is empty, advertise length as 0
	if( message == (struct message*) 0 ){
		int len;
		len = 0;
		write(client_sock, (void*) &len, sizeof(int));
		return;
	}

	// Else, send length of stegotext block plus the block content
	write(client_sock, (void*) &(message->len), sizeof(int));
	write(client_sock, message->data, message->len);

	print_block("DEBUG: stegotext block consumed from main queue: ", 
		message->data, message->len);

	free(message->data);
	free(message);
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
			close(client_sock);
			return;
		case CMD_CONSUME_STXT:
			do_consume_stegotext(client_sock);
			close(client_sock);
			return;
		default:
			print_error("Unknown command received.");
			close(client_sock);
			return;
	}
}


int main(int argc, char** argv){
	// Load OpenSSL components
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();

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
