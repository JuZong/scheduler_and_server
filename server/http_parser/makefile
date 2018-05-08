CC = g++
VAR = release
TEST = http_parser.o

ifeq ($(VAR),debug)
	CFLAG = -c -g
else 
	CFLAG = -c
endif

all : $(TEST)

$(TEST) : http_parser.cpp
	$(CC) $(CFLAG)  $^

clean:
	rm $(TEST)

