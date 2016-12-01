/**
 * lib-steg-mq.c
 *
 * Implementation of a shared library used to interface with 
 * steg-mq.
 */

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

	if((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
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
 * Publishes a stegotext to the message queue.
 * Params:
 *	message - message to hide
 *	key - key to use to hide the message
 */
void publish_stegotext(char* message, char* key){
	int conn;

	conn = getConnection();
	write(conn, CMD_PUBLISH_STXT_str, 1);

	close(conn);
}

void consume_stegotext(void){

}

