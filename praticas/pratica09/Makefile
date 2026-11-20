CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = main
OBJS = main.o cobertura.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c cobertura.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: all clean