SOURCES = main.cpp digester.cpp getDirStats.cpp
CPPC = g++
CPPFLAGS = -c -Wall -O2
LDFLAGS = -lcrypto
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = dirstats

all: $(TARGET)

digester.o: digester.h
getDirStats.o: getDirStats.h digester.h
%.o : %.c
$(OBJECTS): Makefile 

.cpp.o:
	$(CPPC) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $(LDFLAGS) $(OBJECTS)

.PHONY: clean
clean:
	rm -f *~ *.o $(TARGET) 

