OBJS	= main.o chip8.o screen.o
SOURCE	= main.c chip8.c screen.c
HEADER	= chip8.h screen.h
OUT	= chip8
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lncurses

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c

chip8.o: chip8.c
	$(CC) $(FLAGS) chip8.c

screen.o: screen.c
	$(CC) $(FLAGS) screen.c


clean:
	rm -f $(OBJS) $(OUT)
