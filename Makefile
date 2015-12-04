CC=g++

DEBUG=-g

INCLUDES = -Itools/ -Isrc/



CFLAGS= -O3 $(DEBUG)  -march=native -std=c++0x -funroll-loops

MAIN =main


RINCLUDES1 = Camera.cpp RModelHandler.cpp glib.cpp helper.cpp RWindow.cpp RGL.cpp RPostProcessor.cpp
RINCLUDES =  $(addprefix src/, $(RINCLUDES1))
SOURCES= src/$(MAIN).cpp src/utils.cpp $(RINCLUDES)

OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/spunto

ifdef STATIC
  GLIBS= $(LIBRARIES) -lGL -lGLEW -lsfml-network-s -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopenal -lX11 -lX11-xcb -lxcb -lxcb-randr  -lxcb-image -ludev -lpthread -lGLEW -lfreetype -ljpeg -lGL
else
  GLIBS= -lGL -lGLEW -lsfml-graphics -lsfml-window -lsfml-system 	
endif



all: mc 

mc: $(OBJECTS) 
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(GLIBS) $(RLIBS)

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean: 
	rm $(OBJECTS) $(EXECUTABLE)
redo: clean all
