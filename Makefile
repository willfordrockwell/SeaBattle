CFLAGSNET = -lws2_32
CFLAGS =
CC = gcc
SOURCES = seaBattle.c gameLogic.c graphics.c
NETSOURCES = network.c
OBJECTS = $(SOURCES:.c=.o)
OBJECTS += $(NETSOURCES:.c=.o)
EXECUTABLE = seaBattle

all: exec

debug: CFLAGS += -g3
debug: clean
debug: exec

exec: $(OBJECTS) network.o
	$(CC) $(OBJECTS) -o $(EXECUTABLE).exe $(CFLAGS) $(CFLAGSNET)

.PHONY: clean
clean:
	rm *.exe
	rm *.o
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o $@
network.o: network.c network.h
	$(CC) -c $(NETSOURCES) $(CFLAGS) $(CFLAGSNET)