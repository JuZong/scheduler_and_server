CC=g++
VAR=relaese
TESK=loger.o
ifeq ($(VAR),debug)
	CFLAG= -c -g
else 
	CFLAG= -c
endif
all:$(TESK)
loger.o:
	$(CC) $(CFLAG) log.cpp
clean:
	rm -rf *.o $(LIB) *.gch
