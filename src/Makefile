CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: imgprocess

imgprocess: improcess.o
	$(CC) $(CFLAGS) main.o -o imgprocess

improcess.o: improcess.c
	$(CC) $(CFLAGS) -c improcess.c

clean:
	rm -f imgprocess *.o