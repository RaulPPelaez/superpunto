CXX=clang++
CC=$(CXX)

#
CFLAGS=  -std=c++11 -O2 -march=native -pipe -funroll-loops
CXXFLAGS=$(CFLAGS)

LDFLAGS= -lSDL2 -lGL -lGLEW 

GLOBJS=RWindow.o RGL.o RGLHandle.o Camera.o RShadowMapper.o

OBJS= spunto.o App.o $(GLOBJS) RFile.o

all:spunto

spunto:$(OBJS)

clean:
	rm -f $(OBJS)

redo: clean all
