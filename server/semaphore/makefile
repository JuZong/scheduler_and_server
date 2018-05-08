CC = g++
VAR = release
TEST = semaphore.o

ifeq ($(VAR),debug)
	CFLAG = -c -g
else
	CFLAG = -c
endif

all : $(TEST)

$(TEST) : semaphore.cpp
	$(CC) $(CFLAG) $^

clean :
	rm $(TEST)
	
