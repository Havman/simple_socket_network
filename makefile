all: server.x client.x

shared.o: shared.c shared.h

server.o: server.c

client.o: client.c

server.x: server.o shared.o
	$(CC) $(CFLAGS) -o $@ $^

client.x: client.o shared.o
	$(CC) $(CFLAGS) -o $@ $^

clean: 
	rm *.x *.o *.a
