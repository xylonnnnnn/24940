CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = string_list
SOURCES = main.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean