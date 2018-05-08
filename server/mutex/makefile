CC = g++
VAR = release
TEST = mutex.o

ifeq ($(VAR),debug)
	CFLAG = -c -g
else
	CFLAG = -c
endif

all : $(TEST)

$(TEST) : mutex.cpp
	$(CC) $(CFLAG) $^
clean:
	rm *.o	
