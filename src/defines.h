#ifndef DEFINES_H
#define DEFINES_H

#define fori(x,y) for(int i=x; i<y;i++)
#define forj(x,y) for(int j=x; j<y;j++)
#define fork(x,y) for(int k=x; k<y;k++)
#define forl(x,y) for(int l=x; l<y;l++)

#define RANDESP ((double)rand()/(double)RAND_MAX)
#define FLIPACOIN (RANDESP>0.5)

#define PI2 3.1415*2.0
#define PI 3.1415

#define Rmax(a,b) ((a>b)?a:b)
#define Rmin(a,b) ((a<b)?a:b)

#define SQUARE(x) ((x)*(x))


#define TARGET_FPS 30

#define FWIDTH  500
#define FHEIGHT   500

#define DEGREE_TO_RAD (PI/180.0f)


#define CAMSPEED 0.6f
#define MOUSE_SENS 0.25f
#define CAM_ANG_SPEED 2


#endif