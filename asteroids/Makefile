CFLAGS = -I ./include
LIB    = ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGL -pthread -lm #-lXrandr

all: asteroids acpp

asteroids: asteroids.c ppm.c log.c
	gcc $(CFLAGS) asteroids.c log.c -Wall -Wextra $(LFLAGS) -o asteroids

acpp: asteroids.cpp ppm.c log.c
	g++ $(CFLAGS) asteroids.cpp log.c -Wall -Wextra $(LFLAGS) -o acpp

clean:
	rm -f asteroids acpp
	rm -f *.o

