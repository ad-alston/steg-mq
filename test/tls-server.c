/**
 * tls-server.c
 *
 * Simple server which uses TLS 1.2 to receive messages.
 */

#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#define RESPONSE "Generic default response.  Your message was: %s"

// Opens a standard TCP socket on a specified port, returning 
// the socket file descriptor.  Returns -1 if unable to open
// socket.
int openSocket(int port){
	int descriptor;
	struct sockaddr_in addr;
	
	descriptor = socket(PF_INET, SOCK_STREAM, 0);
	
	// Construct address struct
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	// Bind the open socket to the address
	if(bind(descriptor, (struct sockaddr*) &addr, sizeof(addr)) != 0){
		fprintf(stderr, "Unable to bind server socket on port %d \n",
			port);
		return -1;
	}
	if( listen(descriptor, 100) == -1){
		fprintf(stderr, "Unable to listen on server socket.\n");
		return -1;
	}

	return descriptor;
}

// Returns a default SSL context for this server.
// Params:
//	certFile -- .pem file containing the certificate that 
//                  should be used
// Returns a null pointer when unable to construct
// context.
SSL_CTX* getServerContext(char* certFile){
	SSL_METHOD* method;
	SSL_CTX* context;

	OpenSSL_add_all_algorithms();

	SSL_load_error_strings();
	method = TLSv1_2_server_method();

	context = SSL_CTX_new(method);
	if(context != NULL){
		SSL_CTX_set_cipher_list(context, "AES256-SHA");
		
		// Attempt to load certificate
		if( SSL_CTX_use_certificate_file(context, certFile, 
			SSL_FILETYPE_PEM) <= 0 ||
		    SSL_CTX_use_PrivateKey_file(context, certFile,
			SSL_FILETYPE_PEM) <= 0 ||
		    !SSL_CTX_check_private_key(context) ){
			// If error, free the context and return null
			SSL_CTX_free(context);

			return NULL;
		}
	}

	return context;
}

// Serves an SSL connection.
void serveConnection(SSL* ssl){
	char messageBuffer[1024];
	char response[1024];

	int clientSocket, numBytes;

	if( SSL_accept(ssl) != -1 ){
		// Get message length
		numBytes = SSL_read(ssl,messageBuffer,sizeof(messageBuffer)-1);
		if(numBytes > 0){
			// Read and print message
			messageBuffer[numBytes] = 0;
			fprintf(stderr,"Message from client: %s\n",messageBuffer);
			// Form and send response
			sprintf(response, RESPONSE, messageBuffer);
			SSL_write(ssl, response, strlen(response));
		}
	} else{
		fprintf(stderr, "Unable to accept SSL connection.\n");
		ERR_print_errors_fp(stderr);
	}

	// close the connection
	clientSocket = SSL_get_fd(ssl);
	SSL_free(ssl);
	close(clientSocket);
}

int main(int argc, char** argv){
	if(argc != 2){
		printf("Usage: tls-server <port>\n");
		return 0;
	}

	SSL_CTX* context;
	int descriptor;
	
	// Initialize the server context
	SSL_library_init();
	context = getServerContext("testcert.pem");
	if(context == NULL){
		fprintf(stderr, "Unable to create SSL context.\n");
		return -1;
	}

	// Open a server socket
	descriptor = openSocket(atoi(argv[1]));
	if(descriptor == -1){
		return -1;
	}
	
	// Serve forever
	while(1){
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);

		SSL* ssl;

		// Accept next connection
		int client;
		client = accept(descriptor, (struct sockaddr*) &addr, &len);
		printf("Client: %d\n", client);		

		ssl = SSL_new(context);
		SSL_set_fd(ssl, client);
		
		serveConnection(ssl);
	}

	// Technically dead code... should add sighandler
	close(descriptor);
	SSL_CTX_free(context);
}
