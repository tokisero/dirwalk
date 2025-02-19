CC = gcc
CFLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror -D_POSIX_C_SOURCE=200809L 

build/dirwalk: src/dirwalk.c
	$(CC) $(CFLAGS)-o build/dirwalk src/dirwalk.c

clean:
	-D_POSIX_C_SOURCE=200809L rm -f build/* 
