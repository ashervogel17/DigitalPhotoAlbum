# Dependencies
SRCS = driver.c validate_params.c user_input.c album_building.c
OBJS = $(SRCS:.c=.o)
L1 = 
LIBS = 

# Variables
BUILD_DIR = .
EXECUTABLE = album
CFLAGS = -Wall -pedantic -std=c11 -ggdb #-I$(L1)
CC = gcc
MAKE = make

# Targets
.PHONY: clean

$(BUILD_DIR)/$(EXECUTABLE): $(OBJS) $(LIBS)
	echo $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

validate_params.o: validate_params.h
user_input.o: user_input.h
album_building.o: album_building.h

clean:
	rm -f *.o
	rm -f $(BUILD_DIR)/$(EXECUTABLE)