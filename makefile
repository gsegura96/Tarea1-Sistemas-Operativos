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
	mkdir /opt/$(OUT)
	install -m 557 $(OUT) /opt/$(OUT)
	ln -s /opt/$(OUT)/$(OUT) /usr/bin/$(OUT)
	install -m 755 ImageServer.service /etc/systemd/system/ImageServer.service
	install -m 755 server.conf /opt/$(OUT)/
	systemctl enable ImageServer.service

uninstall:
	rm -rf /opt/$(OUT)
	rm /usr/bin/$(OUT)
	systemctl disable ImageServer.service
	rm /etc/systemd/system/ImageServer.service
	
