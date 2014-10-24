CC=gcc
CFLAGS=-O2 -Wall -Wextra -Werror -std=c99

SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
HEADERS=$(patsubst %.c,%.o,$(SOURCES))

PRINT_HEADER_OBJECTS=$(patsubst %,src/%,io.o)
PRINT_HEADER_FILES=$(patsubst %,src/%,gadget_types.h print_header.c)

all: bin/print_header

debug: CFLAGS += -g -D DEBUG
debug: all

bin:
	mkdir -p bin/

io.o: gadget_types.h
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

bin/print_header: bin $(PRINT_HEADER_FILES) $(PRINT_HEADER_OBJECTS)
	$(CC) $(CFLAGS) -I src -o $@ src/print_header.c $(PRINT_HEADER_OBJECTS) -lm

clean:
	rm -r bin/
	rm  src/*.o
