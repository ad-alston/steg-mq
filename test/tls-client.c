/**
 * tls-client.c
 *
 * Simple client which uses TLS 1.2 to communicate with a server.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Attempts to connect to a remote host on a given 
// port.  Returns file descriptor of the socket if successful,
// else -1.
int connectToServer(char* hostname, int port){
	int descriptor;
	
	struct hostent *host;
	struct sockaddr_in addr;

	host = gethostbyname(hostname);
	if(host == NULL){
		fprintf(stderr, "Unable to resolve hostname.\n");
		return 0;
	}

	descriptor = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)(host->h_addr);
	
	if(connect(descriptor, (struct sockaddr*) &addr, sizeof(addr)) != 0){
		close(descriptor);
		fprintf(stderr, "Unable to connect to remote host.\n");
		return -1;
	}

	return descriptor;
}

// Constructs an SSL context for the client.
SSL_CTX* getTLSContext(void){
	SSL_METHOD* method;
	SSL_CTX* context;

	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();	

	method = TLSv1_2_client_method();

	context = SSL_CTX_new(method);

	SSL_CTX_set_cipher_list(context, "AES256-SHA");

	return context;
}

int main(int argc, char** argv){
	if(argc != 3){
		printf("Usage: tls-client <host> <port>\n");
		return 0;
	}

	SSL_library_init();

	SSL_CTX* context;
	context = getTLSContext();

	int socket;
	socket = connectToServer(argv[1], atoi(argv[2]));

	SSL* ssl;
	ssl = SSL_new(context);
	SSL_set_fd(ssl, socket);

	if(SSL_connect(ssl) == -1){
		printf("Usage: unable to complete SSL connection.\n");
		return 0;
	}

	// Write a message to the server
	char* message = "Generic client message.  Hello.";
	SSL_write(ssl, message, strlen(message));

	// Read the server response
	char buffer[1025];
	int numBytes;
	numBytes = SSL_read(ssl, buffer, sizeof(buffer));
	buffer[numBytes] = 0;
	
	printf("Server response: %s\n", buffer);

	close(socket);
	SSL_CTX_free(context);
	return 0;
}
