/**
 * mq.c
 *
 * Main driver for steg-mq.  Listens on the UNIX domain
 * interface and responds to requests accordingly.
 *
 * Aubrey Alston (ada2145@columbia.edu)
 */

#include <stdio.h>
#include <string.h>

#include "stream_interface.h"

/**
 * Serves a client specified by the client socket
 * corresponding to his or her connection.
 */
void serve(int client_sock){
	fprintf(stdout, "Received client: %d\n", client_sock);
}

/**
 * Prints a message to stderr.
 */
void print_error(char* msg){
	fprintf(stderr, "Steg-MQ ERROR: %s\n", msg);
}

int main(int argc, char** argv){
	int intf_sock;

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
