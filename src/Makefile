CXX=g++
CC=$(CXX)

#
CFLAGS=  -std=c++11 -O2 -march=native -pipe -funroll-loops -I../third_party/include -g
CXXFLAGS=$(CFLAGS)


LDFLAGS= -L../third_party/lib
LDLIBS= -lSDL2 -lSDL2_ttf -lGL ../third_party/glew.o -lpng


GLOBJS=RWindow.o RGL.o RGLHandle.o Camera.o  RTextRenderer.o

OBJS= spunto.o App.o $(GLOBJS) RFile.o RPNG.o math_helper.o

all:spunto

spunto:$(OBJS)

clean:
	rm -f $(OBJS)

redo: clean all
