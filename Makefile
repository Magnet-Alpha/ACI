# Basic Makefile
 
CPPFLAGS= `pkg-config --cflags sdl`
CC=clang
CFLAGS=  -std=c99 -O0 -g
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image

GSRC= field.c
GOBJ= ${GSRC:.c=.o}
 
# empty all rule to prevent clean being the default rule
all: field

graph: ${GOBJ}
 
clean:
	rm -f *~ *.o
	rm -f field
 
# END
