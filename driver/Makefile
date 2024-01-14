# Dependencies
SRCS = driver.c ../thumbnail/generate_thumbnail.c
OBJS = $(SRCS:.c=.o)
L1 = ../thumbnail
LIBS = $(L1)/thumbnail.a

# Variables
BUILD_DIR = .
EXECUTABLE = album
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(L1)
CC = gcc
MAKE = make

# Targets
.PHONY: clean

$(BUILD_DIR)/$(EXECUTABLE): $(OBJS) $(LIBS)
	echo $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f $(BUILD_DIR)/$(EXECUTABLE)

album.o: ../thumbnail/generate_thumbnail.h
../thumbnail/generate_thumbnail.o: ../thumbnail/generate_thumbnail.h

