# Basic Makefile
 
CPPFLAGS=
CC=clang
CFLAGS=  -std=c99 -O0 -g
LDFLAGS=
LDLIBS=

GSRC= field.c
GOBJ= ${GSRC:.c=.o}
 
# empty all rule to prevent clean being the default rule
all: field

graph: ${GOBJ}
 
clean:
	rm -f *~ *.o
 
# END
