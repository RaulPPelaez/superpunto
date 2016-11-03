CXX=g++
CC=$(CXX)

#
CFLAGS=  -std=c++11 -Ofast -march=native -pipe -funroll-loops -g #-I../third_party/include
CXXFLAGS=$(CFLAGS)


LDFLAGS= #-L../third_party/lib
LDLIBS= -lSDL2 -lSDL2_ttf -lGL -lpng -lGLEW #../third_party/glew.o 


GLOBJS=RWindow.o RGL.o Camera.o  RTextRenderer.o RRenderer.o RParticleRenderer.o RArrowRenderer.o RBox.o

OBJS= spunto.o App.o $(GLOBJS) RFile.o RPNG.o math_helper.o

all:shaders spunto 

#This transforms the sahder files into strings in a header file shaders.h so there are compiled with the rest of the code
.PHONY: shaders
shaders:
	echo "#ifndef SHADERS_H\n#define SHADERS_H" > shaders.h
	ls shaders --hide=*~ | while read sh; do \
		xxd -i "shaders/$$sh" | sed "s/\([0-9a-f]\)$$/\0, 0x00/" | sed "s/unsigned/const/g" >> shaders.h; \
	done; \
	echo " " >> shaders.h; 
	echo "#endif" >> shaders.h;
spunto:$(OBJS)

clean:
	rm -f $(OBJS) shaders.h

redo: clean all
