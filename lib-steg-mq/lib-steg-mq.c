/**
 * lib-steg-mq.c
 *
 * Implementation of a shared library used to interface with 
 * steg-mq.
 */

#include <stdio.h>
#include "lib-steg-mq.h"
#include "../mq/commands.h"

/** 
 * Obtains a connection to the steg-MQ domain socket.
 * Returns -1 if unsuccessful; otherwise returns socket
 * file descriptor.
 */
int getConnection(void){
	struct sockaddr_un addr;
	int descriptor;

	if( (descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		return -1;
	}
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, STEG_MQ_SOCK_PATH);
	
	if(connect(descriptor, (struct sockaddr *) &addr,
		sizeof(struct sockaddr_un)) < 0){
		
		return -1;
	}

	return descriptor;
}

/**
 * Consumes the next block of stegotext from the 
 * message queue.  Returns the number of bytes read.
 * (0 if no bytes of a block were read)
 * Params:
 *    dest -- memory in which to read the next block
 *    len -- maximum number of bytes to read into 
 *           dest from the stegotext block
 */
int consume_stegotext(void* dest, int len){
	if( len == 0 ) return 0;

	int conn;
	int blockLen;

	if( (conn = getConnection()) == -1){
		return 0;
	}
	
	write(conn, CMD_CONSUME_STXT_str, 1);

	if( read(conn, (void*) &blockLen, sizeof(int)) < sizeof(int) ){
		// Length of block not properly sent
		return 0;
	}	

	if(blockLen == 0) return 0;

	int result;
	result = read(conn, dest, (blockLen < len ? blockLen : len) );

	close(conn);
	
	return result;
}



/**
 * Publishes a stegotext to the message queue.
 * Params:
 *	message - message to hide
 *	key - key to use to hide the message
 *
 * Returns -1 if unable to establish connection to steg-mq
 */
int publish_stegotext(char* message, char* key){
	int conn;
	int message_length;

	message_length = strlen(message);

	if( (conn = getConnection()) == -1 ){
		return -1;
	}

	write(conn, CMD_PUBLISH_STXT_str, 1);
	write(conn, (void*) &message_length, sizeof(int));
	write(conn, message, message_length);
	write(conn, key, 32);
	
	close(conn);

	return 0;
}
