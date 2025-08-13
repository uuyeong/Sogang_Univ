CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = hw2
SRC = hw2.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
