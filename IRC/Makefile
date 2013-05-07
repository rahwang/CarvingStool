OBJS = main.o parse_message.o simclist.o
DEPS = $(OBJS:.o=.d) 
CC = gcc
CFLAGS = -I../../include -g3 -g -Wall -fpic -std=gnu99 -MMD -MP -DDEBUG -DMUTEX
BIN = ../chirc
LDLIBS = -pthread

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(BIN)
	
%.d: %.c %.h

parse_message.o: parse_message.c parse_message.h 

simclist.o: simclist.c simclist.h

clean:
	-rm -f $(OBJS) $(BIN) *.d
