CC=g++

DEBUG=-g

INCLUDES = -Itools/ -Isrc/



CFLAGS= -Ofast -ffast-math $(DEBUG) -march=native -std=c++11 -pipe -funroll-loops

MAIN =main


RINCLUDES1 = Camera.cpp RModelHandler.cpp glib.cpp helper.cpp RWindow.cpp RGL.cpp RPostProcessor.cpp
RINCLUDES =  $(addprefix src/, $(RINCLUDES1))
SOURCES= src/$(MAIN).cpp src/utils.cpp $(RINCLUDES)

OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/spunto

ifdef STATIC
  GLIBS= $(LIBRARIES)  -lGL -lGLEW  -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lX11 -lX11-xcb -lxcb -lxcb-randr  -lxcb-image  -lpthread -lfreetype -ludev -ljpeg
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
