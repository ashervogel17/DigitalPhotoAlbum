.SUFFIXES: .c

SRCS = 
OBJS = $(SRCS:.c=.o)
OUTPUT = gallery

CC = gcc
CFLAGS = 
LIBS = 

all:
	make -C src OUTPUT=../$(OUTPUT) 

clean:
	rm -f $(OBJS) $(OUTPUT)
	make -C src clean

depend:
	makedepend -I/usr/local/include/g++ -- $(CFLAGS) -- $(SRCS) 

# DO NOT DELETE


