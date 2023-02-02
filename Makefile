CC=g++
CFLAGS=-Wall -Wextra -lpthread

build: tema1

tema1: tema1.o utils/parser.cpp utils/helper.cpp utils/perfect_power.cpp
	$(CC) $(CFLAGS) $^ -o $@ -lpthread 

clean: 
	rm -f *.o
	rm tema1