# Superpunto
SDL2/OpenGL4.5 clone of mrevenga's SDL punto ( http://punto.sourceforge.net/ )

With SSAO and superb light effects!!

![alt text](screenshots/shot_0.png "")


## Installation

There are several ways to install and run the project, depending on your operating system and preferences. Below are the instructions for each platform.

### Linux or Windows under WSL (AppImage)

**One-line install**  
You can copy paste this command in your terminal, after which the command "spunto" will be available.
```bash
mkdir -p ~/.local/bin && curl -L https://github.com/RaulPPelaez/superpunto/releases/latest/download/spunto-x86_64.AppImage -o ~/.local/bin/spunto && chmod +x ~/.local/bin/spunto
```
This command will download the latest AppImage, make it executable and store it under `~/.local/bin/`, a folder that is included in the PATH for most distros.

Prebuilt AppImages are generated automatically for each release. You can download the latest version from the [Releases](https://github.com/RaulPPelaez/superpunto/releases) page.

To run:

```bash
chmod +x spunto-x86_64.AppImage
./spunto-x86_64.AppImage
```
Runs on most Linux distributions (glibc ≥ 2.31)

You can place the AppImage somewhere in your PATH for easier access:

```bash
cp spunto-x86_64.AppImage ~/.local/bin/spunto
```
Then you can run it from anywhere.


### Building from Source

If you prefer to build the project yourself, there are multiple supported options:

#### Get the Source Code

```bash
git clone https://github.com/RaulPPelaez/superpunto.git
cd superpunto
```
#### Dependencies
Make sure you have the following dependencies installed:
- CMake
- C++ Compiler (g++, clang++)
- OpenGL 4.5
- SDL2
- SDL2_ttf
- libGLEW
- libPNG
- (Optional) ffmpeg for recording

You can install these dependencies using your package manager. For example, on Ubuntu:

```bash
$ sudo apt install cmake g++ libsdl2-dev libsdl2-ttf-dev libglew-dev libpng-dev ffmpeg
```
Or in Fedora:
```bash 
$ sudo dnf install SDL2-devel SDL2_ttf-devel libpng-devel libGLEW ffmpeg glew-devel
```
Or use the provided `environment.yml` file to create a [Conda](https://github.com/conda-forge/miniforge) environment:

```bash
conda env create -f environment.yml
conda activate spunto-env
```

### Build the Project

Superpunto is built using CMake. You can build it in a separate directory:

```bash
mkdir build
cd build
cmake ..
make -j install
```
If you want to install it system-wide, you will need to run the `make install` command with `sudo`:

```bash
sudo make install
```

If using conda, you must instruct CMake to use the conda paths:

```bash
cmake -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_PREFIX_PATH=$CONDA_PREFIX ..
```

#### Additional Compilation Options

**Build the AppImage**  

If you want to build the AppImage, you can do so by adding the `-DBUILD_APPIMAGE=ON` option to the CMake command:

```bash
cmake -DBUILD_APPIMAGE=ON ..
```

# USAGE
Run with 
```bash
$ ./spunto inputfile [opts]
```

**Run `spunto -h` for information about the available options and controls.**

## Input File
The inputfile should have the following structure:

	#Lx=X;Ly=Y;Lz=Z; Comments are used to separate frames, you can force the size of the simulation box starting the comment as in this example. All three L must be provided
	X1 Y1 Z1 r1 c1 #You can comment here aswell. If your file has more than
	X2 ...         # 5 columns, the rest will be ignored!
	.
	.
	# frame=2
	X1 Y1 Z1 r1 c1
	.
	.
	# frame = 3

r1 is the size of the superpunto. 

c1 is its color.

If some of the columns are missing, this is the behavior according to the number of columns:

	3: XYZ ->r=1, c=0
	4: XYZC -> r=1

### About colors

The column color can be treated in two ways:

**Default** or using --palette X

	The colors are selected using C++ rand(), setting the initial seed to a constant given by --palette (or 923302100 by default), this constant defines a color palette with colors randomly distributed between 0 (black) and 255^3(white). The generated color palette contains 1000 colors.

	The exceptions are id=0 -> red, id=1 ->green, id=2 -> blue


**RGB** by using --RGB

	The colors will be treated as an BGR hexadecimal color, i.e. being 255 = 0xFF = red and 16711680 = 0xFF0000 = blue.
