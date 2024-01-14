.SUFFIXES: .c

SRCS = main.c
OBJS = $(SRCS:.c=.o)
OUTPUT = gallery

CC = gcc
CFLAGS = 
LIBS = 

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(OUTPUT)

depend:
	makedepend -I/usr/local/include/g++ -- $(CFLAGS) -- $(SRCS) 

# DO NOT DELETE


