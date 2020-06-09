all: server2.x client.x

server2.o: server2.c

client.o: client.c

server2.x: server2.o
	$(CC) $(CFLAGS) -o $@ $^

client.x: client.o
	$(CC) $(CFLAGS) -o $@ $^

clean: 
	rm *.x *.o cpy-ball.gif
