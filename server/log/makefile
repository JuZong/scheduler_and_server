CC = g++
VAR = release
TEST = log.o 
ifeq ($(VAR),debug)
	CFLAG = -c -g
else
	CFLAG = -c
endif

all : $(TEST)

$(TEST) : log.cpp 
	$(CC) $(CFLAG) $^

clean :
	rm $(TEST)
