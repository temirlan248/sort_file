TARGET = sort_file
CC = g++
CFLAGS = -c -Wall
LDFLAGS =
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS)