# Superpunto
SFML/Modern OpenGL clone of mrevenga's punto ( http://punto.sourceforge.net/ )

#INSTALLATION
Run the Makefile.
Dependencies:

1. SFML-2.0 and its dependencies. Only libsfml-graphics, libsfml-system and libsfml-window are required
2. OpenGL 3.3+

Known to work in Ubuntu 14. You can install SFML by $ apt-get install libsfml-dev
Precompiled sfml libs can be found under tools.
#USAGE
Use with $ ./superpunto inputfile

inputfile should have the following structure:
>\# Comments are used to separate frames

>X1 Y1 Z1 r1 c1

>X2 ...

>.

>.

>.

>\# frame=2

>X1 Y1 Z1 r1 c1

>.

>.

>.

>\# frame = 3

r1 is the size of the superpunto.

c1 is its color.

#COLORS
The colors are RGB encoded as an int number varying from 0 to 255^3 = 16581375 as c = Decimal(0xRRGGBB)

Where RR/GG/BB go from 0x00=0 to 0xFF=255

i.e full red (255,0,0) would be 0xFF0000=16711680

#CONTROLS
You can move around using:

1. WASD for moving in your XY plane
2. LSHIFT and Lctrl to go up and down
3. E and Q to roll
4. Keep LAlt pressed to look around using the mouse
5. Move faster/slower using + -
6. Numbers 1-6 to rotate the system in the X, Y o Z axis 

You can go through time using:

1. Space/R to go to the next/previous frame
2. Press M to autoplay the frames at 30 FPS

You can take screenshots and record some frames in a gif using:

1. Press L to take a screenshot every frame, these will be converted to gif at exit. You can move around while recording
2. Press C to take a single screenshot.






