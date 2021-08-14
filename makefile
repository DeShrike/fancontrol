CC=gcc
AR=ar
LIBS=-lwiringPi
CFLAGS=-Wall -O3
OBJ=fancontrol.o ini.o config.o

all: fancontrol

rmbin:
	rm fancontrol

fancontrol: $(OBJ)
	$(CC) -o fancontrol $(OBJ) $(LIBS)

fancontrol.o: fancontrol.c config.h fancontrol.h
	$(CC) -c $(CFLAGS) fancontrol.c

config.o: config.c config.h fancontrol.h
	$(CC) -c $(CFLAGS) config.c

ini.o: ini.c ini.h
	$(CC) -c $(CFLAGS) ini.c

clean:
	rm *.o

install: fancontrol fancontrol.service
	sudo cp fancontrol /usr/bin
	sudo cp fancontrol.service /etc/systemd/system
	sudo cp fancontrol.ini /etc
	sudo systemctl daemon-reload

uninstall: fancontrol fancontrol.service
	sudo systemctl stop fancontrol
	sudo systemctl disable fancontrol
	sudo rm /usr/bin/fancontrol
	sudo rm /etc/systemd/system/fancontrol.service
	sudo rm /etc/shutdownpi.ini
	sudo systemctl daemon-reload

start:
	sudo systemctl start fancontrol

stop:
	sudo systemctl stop fancontrol

enable:
	sudo systemctl enable fancontrol

disable:
	sudo systemctl disable fancontrol
