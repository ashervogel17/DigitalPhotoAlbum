# Dependencies
SRCS = driver.c ../photo_lib/photo_lib.c validate_params.c
OBJS = $(SRCS:.c=.o)
L1 = ../photo_lib
LIBS = $(L1)/photo_lib.a

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