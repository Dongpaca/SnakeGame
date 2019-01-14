# 작성자: 배정민(20133079)
# make main : compile and link (build)
# make clean : remove all object files in directory

CC = g++
CCFLAGS = -g

main: main.o snake.o board.o
	$(CC) $(CCFLAGS) -o main main.o snake.o board.o -lncurses

clean:
	rm -f *.o

%.o : %.cpp %.h
	$(CC) $(CCFLAGS) -c $<

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $<

% : %.cpp
	$(CC) $(CCFLAGS) -o $@ $<
