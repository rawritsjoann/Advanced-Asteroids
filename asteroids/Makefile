CFLAGS = -I ./include
LIB    = ./libggfonts.so
#LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm -g #-lXrandr

all: asteroids


asteroids: asteroids.cpp ppm.c log.c
	g++ $(CFLAGS) asteroids.cpp log.c -Wall -Wextra $(LFLAGS) -o asteroids ./libggfonts.so
#asteroids2: asteroids2.cpp ppm.c log.c
#	g++ $(CFLAGS) asteroids2.cpp log.c -Wall -Wextra $(LFLAGS) -o asteroids2

acpp: asteroids.cpp ppm.c log.c
	g++ $(CFLAGS) ppm.c asteroids.cpp log.c -Wall -Wextra $(LFLAGS) -o acpp

clean:
	rm -f asteroids 
	rm -f *.o

