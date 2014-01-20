OBJS = floyd_serial.o stopwatch.o
DEPS = $(OBJS:.o=.d)
CC = gcc
CFLAGS = -Wall -Werror -O3
BIN = floyd_serial
LDLIBS =  -pthread

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(BIN)

%.d: %.c

clean:
	-rm -f $(OBJS) $(BIN) *.o
