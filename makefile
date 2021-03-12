OBJS	= main.o  log.o
SOURCE	= main.c log.c
HEADER	= config.h stb_image.h uini.h stb_image_write.h log.h
OUT	= ImageServer
CC	 = gcc
FLAGS	 = -g -c -Wall -std=c17 
LFLAGS	 = -lm -ltiff -ljpeg -lpng -pthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)
