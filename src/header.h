#ifndef HEADER_H
#define HEADER_H

#include<SDL2/SDL.h>
#define GL3_PROTOTYPES 1
#include"GL/glew.h"
#include<string>
typedef unsigned int uint;


#define IF_KEY(KEY, foo)  if(e.key.keysym.sym == SDLK_##KEY){foo}

extern uint FWIDTH; 
extern uint FHEIGHT;

#define RANDESP (rand()/(float)RAND_MAX)

#define Rmax(x,y) ((x)>(y)?(x):(y))

#define fori(x,y) for(int i=x; i<y; i++)
#define forj(x,y) for(int j=x; j<y; j++)


#define MOUSE_SENS 0.25f



#endif
extern uint palette_id;
