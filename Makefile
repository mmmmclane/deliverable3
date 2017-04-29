SHELL = /bin/sh
FLAGS = -Wall
CC = g++

all:	echo_c echo_s log_s

echo_c:		echo_c.c
	$(CC) $(FLAGS) -o $@ echo_c.c

echo_s:		echo_s.c
	$(CC) $(FLAGS) -o $@ echo_s.c
	
log_s:		log_s.c
	$(CC) $(FLAGS) -o $@ log_s.c
	
clean:
	rm echo_c echo_s log_s
