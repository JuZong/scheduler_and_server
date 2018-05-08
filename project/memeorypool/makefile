CC=g++
VAR=relaese
TESK=scheuler.o
ifeq ($(VAR),debug)
	CFLAG= -c -g
else 
	CFLAG= -c
endif
all:$(TESK)
scheuler.o:
	$(CC) $(CFLAG) scheuler.cpp
clean:
	rm -rf *.o $(LIB) *.gch
