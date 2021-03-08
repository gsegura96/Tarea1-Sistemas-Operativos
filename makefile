OBJS	= main.o 
SOURCE	= main.c
HEADER	= config.h stb_image.h uini.h
OUT	= ImageServer
CC	 = gcc
FLAGS	 = -g -c -Wall -std=c17 
LFLAGS	 = -lm -ltiff -ljpeg -lpng -pthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)