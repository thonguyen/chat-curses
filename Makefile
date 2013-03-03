CC=gcc
CFLAGS=-Wall 
LDFLAGS=-lncurses
SOURCES=window.c chat.c csocket.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=chat

all: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

