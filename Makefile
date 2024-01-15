# Variables
BUILD_DIR = ..
EXECUTABLE = album
DRIVER_VARS = BUILD_DIR=$(BUILD_DIR) EXECUTABLE=$(EXECUTABLE)

# Targets
.PHONY = all clean

all:
	make -C photo_lib
	make -C driver $(DRIVER_VARS)

clean:
	make -C photo_lib clean
	make -C driver clean $(DRIVER_VARS)