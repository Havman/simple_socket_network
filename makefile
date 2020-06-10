all: server.x client.x

server.o: server.c

client.o: client.c

server.x: server.o
	$(CC) $(CFLAGS) -o $@ $^

client.x: client.o
	$(CC) $(CFLAGS) -o $@ $^

clean: 
	rm *.x *.o cpy-ball.gif
