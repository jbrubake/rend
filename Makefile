CC=gcc
CFLAGS=-Iinclude -DWINDOWS -DDEBUG -c -Wall -g
LDFLAGS=-static-libgcc -Llib32 -lmingw32 -mconsole
SOURCES= \
src/interface.c \
src/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=poc.exe

.PHONY: all clean test

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) $< -o $@
