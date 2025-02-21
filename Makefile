CC = gcc
CFLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror 

build/dirwalk: src/dirwalk.c
	$(CC) $(CFLAGS)-o build/dirwalk src/dirwalk.c

clean:
	rm -f build/* 
