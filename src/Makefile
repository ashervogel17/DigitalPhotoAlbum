# Dependencies
SRCS = driver.c validate_params.c user_input.c write_html.c ../photo_lib/photo_lib.c
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
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f $(BUILD_DIR)/$(EXECUTABLE)