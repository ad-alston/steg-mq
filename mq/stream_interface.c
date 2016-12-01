/**
 * stream_interface.c
 *
 * Implements functionality required to set up, listen on,
 * and close the UNIX domain socket interface to steg-mq.
 */

#include "stream_interface.h"
#include <stdio.h>
#include <string.h>

/**
 * Creates, binds, and listens on a UNIX domain socket.
 *
 * returns the created socket or -1 if some action has failed
 */
int interface_start(void){
	struct sockaddr_un addr;
	int sock_descriptor;

	if( (sock_descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 ){
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SMQ_SOCK_PATH);
	unlink(addr.sun_path);

	if( bind(sock_descriptor, (struct sockaddr*) &addr,
			sizeof(struct sockaddr_un)) < 0 ){
		return -1;
	}

	if( listen(sock_descriptor, MAX_CONNECTIONS) < 0 ){
		return -1;
	}

	return sock_descriptor;
}

/**
 * Closes a passed UNIX domain socket.
 *
 * params:
 *	descriptor - file descriptor of socket to close
 */
int interface_close(int descriptor){
	close(descriptor);
}

/**
 * Reads n bytes from a socket stream into buffer.
 */
int readBytes(int socket, int n, void* buffer){
	int read = 0;
	int result;

	while(read < n){
		if( (result = read(socket, buffer + read, n - read) < 1)){
			return -1;
		}
		read += result;
	}
	return 0;
}

