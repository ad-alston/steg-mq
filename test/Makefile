tls-server: tls-server.c
	gcc -Wall -o tls-server tls-server.c -L/usr/local/lib -lssl -lcrypto
tls-client: tls-client.c
	gcc -Wall -o tls-client tls-client.c -L/usr/local/lib -lssl -lcrypto
test-client-server: tls-server tls-client
