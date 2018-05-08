CC = g++
VAR = release
TEST = main

ifeq ($(VAR),debug)
	CFLAG = -c -g
else
	CFLAG = -c
endif
all : main

main : main.o server.o ../log/log.o ../mutex/mutex.o ../http_parser/http_parser.o ../http_splicer/http_splicer.o ../semaphore/semaphore.o
	$(CC) -o $@  $^ -ljson -lhiredis

main.o : main.cpp
	$(CC) $(CFLAG) main.cpp

server.o : server.cpp
	$(CC) $(CFLAG) server.cpp

log.o : ../log/log.cpp
	make -C ../log

mutex.o : ../mutex/mutex.cpp
	make -C ../mutex

http_parser.o : ../http_parser/http_parser.h
	make -C ../http_parser

http_splicer.o : ../http_splicer/http_splicer.h
	make -C ../http_splicer

semaphore.o : ../semaphore/semaphore.cpp
	make -C ../semaphore

clean :
	make clean -C ../log
	make clean -C ../mutex
	make clean -C ../http_parser
	make clean -C ../http_splicer
	make clean -C ../semaphore
	
	
