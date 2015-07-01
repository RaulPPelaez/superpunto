CC=g++

DEBUG=-g



#INCLUDES= -IC:/SFML-2.1/include  -IC:/glew-1.11.0/include

INCLUDES = -Itools/

CFLAGS=  -O3 $(DEBUG) -std=c++11

MAIN =main

SOURCES= src/$(MAIN).cpp src/glib.cpp src/RModelHandler.cpp src/Camera.cpp src/utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/mcGUI

GLIBS=   -lGLU -lglut -lGL -lGLEW  -lsfml-graphics -lsfml-window -lsfml-system 


all: mc 

mc: $(OBJECTS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJECTS) $(GLIBS) src/tiny_obj_loader.cc.o

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< $(GLIBS) -o $@


clean: 
	rm $(OBJECTS) $(EXECUTABLE)
redo: clean all
