# Makefile - Pratica 05 (Coloracao de Grafos: Guloso e Welsh-Powell)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pipe -lm
TARGET = main
SRC = *.c
OBJ = *.o

all: $(TARGET)

$(TARGET): $(SRC)
	TMP=/tmp TEMP=/tmp $(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) $(OBJ)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run