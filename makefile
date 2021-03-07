OBJS	= main.o config.o
SOURCE	= main.c config.c
HEADER	= config.h
OUT	= ImageServer
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c -std=c17

config.o: config.c
	$(CC) $(FLAGS) config.c -std=c17

clean:
	rm -f $(OBJS) $(OUT)