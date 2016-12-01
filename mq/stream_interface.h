/**
 * stream_interface.h
 *
 * Header defining dependencies and function signatures
 * for the interface to steg-mq.
 * 
 * Aubrey Alston (ada2145@columbia.edu)
 */

#ifndef __STREAM_INTERFACE_H__
#define __STREAM_INTERFACE_H__ 

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#define SMQ_SOCK_PATH "/tmp/smq_sock"
#define MAX_CONNECTIONS 15

/**
 * Creates, binds, and listens on a UNIX domain socket.
 *
 * returns the created socket or -1 if some action has failed
 */
int interface_start(void);

/**
 * Closes a passed UNIX domain socket.
 *
 * params:
 *	descriptor - file descriptor of socket to close
 */
int interface_close(int descriptor);

/**
 * Reads n bytes from a socket stream into buffer.
 */
int readBytes(int socket, int n, void* buffer);

#endif
