CC=g++ 
LDFLAGS=-std=c++11 -O3
HEADER=src/object.h src/base.h src/bstar.h src/horizontal_contour.h src/globalplacer.h src/sa.h src/TCG.h
SOURCES=src/main.cpp $(HEADER:.h=.cpp)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test

all: $(SOURCES) bin/$(EXECUTABLE)

bin/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c $(HEADER)
	$(CC) $< -o $@

.PHNOY: clean

clean:
	-rm -rf src/*.o src/*/*.o bin/$(EXECUTABLE) 