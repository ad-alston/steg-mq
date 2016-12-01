tls-server: tls-server.c
	gcc -Wall -o tls-server tls-server.c -L/usr/local/lib -lssl -lcrypto

tls-client: tls-client.c
	gcc -Wall -o tls-client tls-client.c -L/usr/local/lib -lssl -lcrypto

lstegmq-test: libstegmq-test.c
	gcc -Wall libstegmq-test.c -o lstegmq-test -L/usr/local/lib -lstegmq

test-client-server: tls-server tls-client
