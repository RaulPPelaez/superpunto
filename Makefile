CC=g++

#DEBUG=-g

INCLUDES = -Itools/

CFLAGS=  -O3 $(DEBUG) -std=c++0x

MAIN =main

SOURCES= src/$(MAIN).cpp src/glib.cpp src/RModelHandler.cpp src/Camera.cpp src/utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/mcGUI

GLIBS= -lglut -lGL -lGLEW  -lsfml-graphics -lsfml-window -lsfml-system 


all: mc 

mc: $(OBJECTS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJECTS) $(GLIBS) src/tiny_obj_loader.cc.o

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< $(GLIBS) -o $@


clean: 
	rm $(OBJECTS) $(EXECUTABLE)
redo: clean all
