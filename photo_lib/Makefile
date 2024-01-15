# Dependencies
SRCS = photo_lib.c
OBJS = $(SRCS:.c=.o)
LIB = photo_lib.a

# Variables
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

# Targets
.PHONY: clean

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

clean:
	rm -f $(LIB) *.o