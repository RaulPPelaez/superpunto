CXX=g++
CC=$(CXX)

#
CFLAGS=  -std=c++11 -O2 -march=native -pipe -funroll-loops -I../third_party/include
CXXFLAGS=$(CFLAGS)


LDFLAGS= -L../third_party/lib
LDLIBS= -lSDL2 -lGL ../third_party/glew.o -lpng

GLOBJS=RWindow.o RGL.o RGLHandle.o Camera.o RShadowMapper.o 

OBJS= spunto.o App.o $(GLOBJS) RFile.o RPNG.o

all:spunto

spunto:$(OBJS)

clean:
	rm -f $(OBJS)

redo: clean all
