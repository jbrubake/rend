CC=gcc
CFLAGS=-Iinclude -DWINDOWS -DDEBUG -DMEMWATCH -c -Wall -g
LDFLAGS=-static-libgcc -Llib -lpdcurses -lmingw32 -mconsole
SOURCES= \
src/kiss.c \
src/memwatch.c \
src/containers.c \
src/fov.c \
src/game.c \
src/interface.c \
src/map.c \
src/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=poc.exe

.PHONY: all clean test

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) memwatch.log

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

test-varr: src/memwatch.o
	$(CC) $(CFLAGS) -DUNITVARR src/containers.c -o src/containers.o
	$(CC) -o $@ src/memwatch.o src/containers.o $(LDFLAGS)

test-llist: src/memwatch.o
	$(CC) $(CFLAGS) -DUNITLLIST src/containers.c -o src/containers.o
	$(CC) -o $@ src/memwatch.o src/containers.o $(LDFLAGS)

test-ref: src/memwatch.o
	$(CC) $(CFLAGS) -DUNITREF src/containers.c -o src/containers.o
	$(CC) -o $@ src/memwatch.o src/containers.o $(LDFLAGS)

test-heap: src/memwatch.o
	$(CC) $(CFLAGS) -DUNITHEAP src/containers.c -o src/containers.o
	$(CC) -o $@ src/memwatch.o src/containers.o $(LDFLAGS)

test-reflist: src/memwatch.o
	$(CC) $(CFLAGS) -DUNITREFLIST src/containers.c -o src/containers.o
	$(CC) -o $@ src/memwatch.o src/containers.o $(LDFLAGS)

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) $< -o $@
