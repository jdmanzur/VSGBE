CC=g++
EXECUTABLE=vsgbe
SOURCES=$(wildcard src/*.cpp)
INCLUDE="/usr/include/SDL2/"
OBJS=$(SOURCES: .cpp=.o)
LIBS= -lSDL2

ifneq (,$(findstring release, $(MAKECMDGOALS)))
CFLAGS=-g -Wall -O2 -s
else
CFLAGS=-g -Wall -ansi -pedantic
endif

debug: $(EXECUTABLE)
release: $(EXECUTABLE)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(OBJS) -g -I$(INCLUDE) $(LIBS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $(LIBS) $< -o $@

clean:
	-rm -f vsgbe
