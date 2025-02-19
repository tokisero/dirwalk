CC = gcc
CFLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror
TARGET = dirwalk

$(TARGET): dirwalk.c
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -o $(TARGET) dirwalk.c

clean:
	-D_POSIX_C_SOURCE=200809L rm -f $(TARGET)
