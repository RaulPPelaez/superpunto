#ifndef DEFINES_H
#define DEFINES_H

typedef unsigned int uint;

#define IF_KEY(KEY, foo)  if(e.key.keysym.sym == SDLK_##KEY){foo}

#define fori(x,y) for(int i=x; i<y; i++)
#define forj(x,y) for(int j=x; j<y; j++)

#endif
