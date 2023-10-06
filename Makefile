CC=gcc
CCFLAGS=-g -D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror

all: zombie controller labo

zombie : zombie.o utils_v2.o network.o
	$(CC) $(CCFLAGS) -o zombie zombie.o utils_v2.o network.o network.h 

controller : controller.o utils_v2.o network.o
	$(CC) $(CCFLAGS) -o controller controller.o utils_v2.o network.o

labo : labo.o utils_v2.o network.o
	$(CC) $(CCFLAGS) -o labo labo.o utils_v2.o network.o

zombie.o: zombie.c utils_v2.h network.h
	$(CC) $(CCFLAGS) -c zombie.c network.h

controller.o: controller.c utils_v2.h network.h
	$(CC) $(CCFLAGS) -c controller.c network.h

labo.o: labo.c utils_v2.h network.h
	$(CC) $(CCFLAGS) -c labo.c network.h

utils_v2.o: utils_v2.c utils_v2.h
	$(CC) $(CCFLAGS) -c utils_v2.c network.h

network.o: network.c network.h utils_v2.h
	$(CC) $(CCFLAGS) -c network.c network.h utils_v2.h

clear :
		clear

clean :
	rm -f *.o
	rm -f *.gch
	rm -f zombie
	rm -f controller
	rm -f labo
	rm -f network