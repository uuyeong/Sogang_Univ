# makefile

CC = g++

CFLAGS = -std=c++11

LIBS = -lpng

TARGET = CP2_2023_FALL_영원_TERM01

SRCS = CP2_2023_FALL_영원_TERM01.cpp

OBJS = CP2_2023_FALL_영원_TERM01.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

output.png: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) output.png
