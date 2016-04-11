CXX=g++
CC=$(CXX)

#
CFLAGS=  -std=c++11 -O2 -march=native -pipe -funroll-loops -m64
CXXFLAGS=$(CFLAGS)

SDLLIBS= $(shell sdl2-config --static-libs)
LDLIBS= -lGL  -lGLEW $(SDLLIBS)

GLOBJS=RWindow.o RGL.o RGLHandle.o Camera.o RShadowMapper.o

OBJS= spunto.o App.o $(GLOBJS) RFile.o

all:spunto

spunto:$(OBJS)

clean:
	rm -f $(OBJS)

redo: clean all
