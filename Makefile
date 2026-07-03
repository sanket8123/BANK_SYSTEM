CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = bank_system

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	del /Q $(TARGET).exe 2>NUL || exit 0

.PHONY: all run clean
