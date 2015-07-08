CC=g++

DEBUG=-DNDEBUG

INCLUDES = -Itools/



CFLAGS=  -Ofast $(DEBUG) -march=native -std=c++0x

MAIN =main

SOURCES= src/$(MAIN).cpp src/glib.cpp src/RModelHandler.cpp src/Camera.cpp src/utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/spunto

GLIBS= $(LIBRARIES) -lGL -lGLEW -lsfml-graphics -lsfml-window -lsfml-system 


all: mc 

mc: $(OBJECTS) 
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(GLIBS)

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean: 
	rm $(OBJECTS) $(EXECUTABLE)
redo: clean all
