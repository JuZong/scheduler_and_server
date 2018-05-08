CC=g++
VAR=relaese
TESK=threadpool.o
ifeq ($(VAR),debug)
	CFLAG=-c -g
else 
	CFLAG=-c
endif
all:$(TESK)
scheuler.o:
	$(CC) $(CFLAG) threadpool.cpp
clean:
	rm -rf *.o $(LIB) *.gch
