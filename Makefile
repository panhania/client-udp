CC=gcc
CFLAGS=-std=c99 -Wall -pedantic

client-udp: client-udp.c utils.o networking.o
	$(CC) $(CFLAGS) -o $@ $^ -lrt

utils.o: utils.c
	$(CC) $(CFLAGS) -c -o $@ $<

networking.o: networking.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o

distclean: clean
	rm -f client-udp
