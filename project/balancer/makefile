CC=g++
VAR=relaese
#LIB=libbalancer.a
Test=main
ifeq ($(VAR),debug)
	CFLAG=-c -g
else 
	CFLAG=-c -g
endif
all:$(Test)
$(Test):scheuler.o epoll.o consistent_hash.o socketpair.o md5.o ../log/log.o ../mutex/mutex.o ../threadpool/threadpool.o -lpthread -ljson 
	$(CC) $^ main.cpp -o main -g
epoll.o: epoll/epoll.cpp
	$(CC) $(CFLAG) epoll/epoll.cpp
consistent_hash.o: consis_hash/consistent_hash.cpp
	$(CC) $(CFLAG) $^
md5.o:consis_hash/md5.cpp
	$(CC) $(CFLAG) $^
socketpair.o: socketpair/socketpair.cpp
	$(CC) $(CFLAG) socketpair/socketpair.cpp
scheuler.o:
	$(CC) $(CFLAG) scheuler/scheuler.cpp
../log/log.o: ../log
	make -C ../log
../muetx/mutex.o: ../mutex
	make -C ../mutex
../threadpool/threadpool.o: ../threadpool
	make -C ../threadpool
clean:
	make clean -C ../log
	make clean -C ../mutex
	make clean -C ../threadpool
	rm -rf *.o $(LIB) *.gch
