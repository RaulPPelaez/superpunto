CC=g++

DEBUG=-g

INCLUDES = -Itools/ -Isrc/



CFLAGS= -O3 $(DEBUG)  -march=native -std=c++0x -funroll-loops

MAIN =main


RINCLUDES1 = Camera.cpp RModelHandler.cpp glib.cpp helper.cpp RWindow.cpp RGL.cpp
RINCLUDES =  $(addprefix src/, $(RINCLUDES1))
SOURCES= src/$(MAIN).cpp src/utils.cpp $(RINCLUDES)

OBJECTS = $(SOURCES:.cpp=.o)


EXECUTABLE=bin/mc

GLIBS= $(LIBRARIES) -lGL -lGLEW -lsfml-graphics -lsfml-window -lsfml-system 
#RLIBS = -lCamera -lglib -lRModelHandler -lhelper -lRWindow


all: mc 

mc: $(OBJECTS) 
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(GLIBS) $(RLIBS)

.cpp.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean: 
	rm $(OBJECTS) $(EXECUTABLE)
redo: clean all
