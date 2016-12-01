steg-mq: mq/mq.c mq/stream_interface.c mq/stream_interface.h
	gcc -o steg-mq mq/mq.c mq/stream_interface.c
libstegmq.so: lib-steg-mq/lib-steg-mq.c lib-steg-mq/lib-steg-mq.h
	gcc -c -Wall -fpic lib-steg-mq/lib-steg-mq.c
	gcc -shared -o libstegmq.so lib-steg-mq.o

.PHONY:
install: steg-mq libstegmq.so
	sudo mkdir -p /usr/local/include/steg-mq
	cp lib-steg-mq/lib-steg-mq.h /usr/local/include/steg-mq/lib-steg-mq.h
	cp libstegmq.so /usr/local/lib/libstegmq.so
	ldconfig
clean:
	rm -f *.o
	rm -f *.so
