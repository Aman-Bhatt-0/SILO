CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -pthread -D_GNU_SOURCE
LDFLAGS = -lcap

SRC = src/main.c src/child.c src/mount.c src/cap.c
OBJ = $(SRC:.c=.o)
DEPS = src/child.h src/mount.h src/cap.h

TARGET = mycontainer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

