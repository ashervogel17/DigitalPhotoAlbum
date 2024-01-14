# Dependencies
SRCS = generate_thumbnail.c
OBJS = $(SRCS:.c=.o)
LIB = thumbnail.a

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

generate_thumbnail.o: generate_thumbnail.h

