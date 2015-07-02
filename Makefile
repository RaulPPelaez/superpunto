CC=g++

#DEBUG=-g

INCLUDES = -Itools/



CFLAGS=  -O2 $(DEBUG) -std=c++0x

MAIN =main

SOURCES= src/$(MAIN).cpp src/glib.cpp src/RModelHandler.cpp src/Camera.cpp src/utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/mcGUI

GLIBS= $(LIBRARIES) -lglut -lGL -lGLEW -lsfml-graphics -lsfml-window -lsfml-system 


all: mc 

mc: $(OBJECTS) 
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(GLIBS)

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean: 
	rm $(OBJECTS) $(EXECUTABLE)
redo: clean all
