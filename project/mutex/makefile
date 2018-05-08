CC=g++
VAR=relaese
TESK=mutex.o
ifeq ($(VAR),debug)
	CFLAG= -c -g
else 
	CFLAG= -c
endif
all:$(TESK)
mutex.o:
	$(CC) $(CFLAG) mutex.cpp
clean:
	rm -rf *.o $(LIB) *.gch
