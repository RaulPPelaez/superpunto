CXX=g++
CC=$(CXX)

MAXLOGLEVEL=5

#-march=native
CFLAGS=  -std=c++11 -Ofast -pipe -funroll-loops -g -I . -DMAXLOGLEVEL=$(MAXLOGLEVEL) #-I../third_party/include
CXXFLAGS=$(CFLAGS)


LDFLAGS= #-L../third_party/lib
LDLIBS= -lSDL2 -lSDL2_ttf -lGL -lpng -lGLEW -lpthread  #../third_party/glew.o 


GLOBJS=RWindow.o RGL.o Camera.o  RTextRenderer.o RRenderer.o RParticleRenderer.o RBox.o # RArrowRenderer.o

OBJS= main.o App.o $(GLOBJS) RFile.o RPNG.o math_helper.o System.o

all:shaders main
	mv main spunto

#This transforms the shader files into strings in a header file shaders.h so there are compiled with the rest of the code
.NOTPARALLEL:

shaders: shaders.h
shaders.h: shaders/*
	printf "#ifndef SHADERS_H\n#define SHADERS_H\n" > shaders.h
	ls shaders --hide=*~ | while read sh; do \
		xxd -i "shaders/$$sh" | sed "s/\([0-9a-f]\)$$/\0, 0x00/" | sed "s/unsigned/const/g" >> shaders.h; \
	done; \
	echo " " >> shaders.h; 
	echo "#endif" >> shaders.h;


main:$(OBJS) 

App.o: 

clean:
	rm -f $(OBJS)  spunto  shaders.h

redo: clean all
