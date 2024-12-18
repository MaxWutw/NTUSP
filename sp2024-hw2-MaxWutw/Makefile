all: friend

friend: friend.o utility.o
	gcc $^ -o $@

friend.o: friend.c hw2.h
	gcc -c friend.c

utility.o: utility.c utility.h
	gcc -c utility.c

clean:
	rm *.o friend
