OBJS	= main.o config.o 
SOURCE	= main.c config.c
HEADER	= config.h stb_image.h
OUT	= ImageServer
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm -ltiff -ljpeg -lpng

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)