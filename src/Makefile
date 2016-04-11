CXX=g++
CC=$(CXX)

#
CFLAGS=  -std=c++11 -O2 -march=native -pipe -funroll-loops -DGLEW_STATIC
CXXFLAGS=$(CFLAGS)

LDFLAGS= -L/usr/lib64
LDLIBS= -lSDL2 -lGL -l:libGLEW.a 

GLOBJS=RWindow.o RGL.o RGLHandle.o Camera.o RShadowMapper.o

OBJS= spunto.o App.o $(GLOBJS) RFile.o

all:spunto

spunto:$(OBJS)

clean:
	rm -f $(OBJS)

redo: clean all
