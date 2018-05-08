CC = g++
VAR = release
TEST = http_splicer.o

ifeq ($(VAR),debug)
	CFLAG = -c -g
else
	CFLAG = -c
endif

all : $(TEST)

$(TEST):http_splicer.cpp
	$(CC) $(CFLAG) $^

clean:
	rm $(TEST)


