#ifndef __LIB_STEG_MQ_H__
#define __LIB_STEG_MQ_H__

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#define STEG_MQ_SOCK_PATH "/tmp/smq_sock"

extern int publish_stegotext(char* message, char* key);
extern int consume_stegotext(void* dest, int len);

#endif
