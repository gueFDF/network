cc =g++
CFLAGS=-Wall -g -o -std==gnu99 -lmuduo_net -lmuduo_base\
-pthread


FILE=EventLoop.cc test.cc channel.cc

EXE_NAME=mymuduo

.PHONY:make

make:
	$(cc) $(FILE) $(CFLAGS) -o $(EXE_NAME)

clean:
	rm -rf  $(EXE_NAME)
