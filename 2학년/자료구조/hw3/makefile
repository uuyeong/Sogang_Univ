CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = hw3
SRC = hw3.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

