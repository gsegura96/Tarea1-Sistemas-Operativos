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
install:
	mkdir /etc/server
	install -m 557 $(OUT) /etc/server/
	ln -s /etc/server/$(OUT) /usr/bin/$(OUT)
	install -m 755 ImageServer.service /etc/systemd/system/ImageServer.service
	install -m 755 server.conf /etc/server/
	systemctl enable ImageServer.service

uninstall:
	rm -rf /etc/server
	rm /usr/bin/$(OUT)
	systemctl disable ImageServer.service
	rm /etc/systemd/system/ImageServer.service
	
