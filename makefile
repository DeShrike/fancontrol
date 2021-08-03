CC=gcc
AR=ar

all: fancontrol

rmbin:
	rm fancontrol

fancontrol: fancontrol.o ini.o config.o
	$(CC) -o fancontrol fancontrol.o ini.o config.o -l wiringPi

fancontrol.o: fancontrol.c config.h fancontrol.h
	$(CC) -c -Wall -O3 fancontrol.c

config.o: config.c config.h fancontrol.h
	$(CC) -c -Wall -O3 config.c

ini.o: ini.c ini.h
	$(CC) -c -Wall -O3 ini.c

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
