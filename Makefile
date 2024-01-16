# Variables
BUILD_DIR = ..
EXECUTABLE = album
SRC_VARS = BUILD_DIR=$(BUILD_DIR) EXECUTABLE=$(EXECUTABLE)

# Targets
.PHONY = all clean

all:
	make -C photo_lib
	make -C src $(SRC_VARS)

clean:
	make -C photo_lib clean
	make -C src clean $(DRIVER_VARS)