# Voltage Simulation
A GLSL powered voltage simulation using a numerical implementation of the Laplace equation

![cool gif of it working](https://raw.githubusercontent.com/nodatapoints/voltage-simulation/master/doc/img/title.gif)

### Installation
This code is expected to run on a linux machine with `g++` installed and GL version `>=4.4`. Additionally, having a GPU makes it more fun.<br>
Install the required packages
```
$ sudo apt install make libsfml-dev libgl1-mesa-dev libglew-dev
```
Make
```
$ cd voltage-simulation/
voltage-simulation$ make
```
that's it.

## Usage
To simulate a setting defined in a file, use
```
./volt [OPTIONS] [FILE]
```
**Important:** the binary needs to be executed from the source foulder in order to load the shaders.

| Option       | Type    | Description                                                               |
|:------------:|:-------:| ------------------------------------------------------------------------- |
| `-a <value>` | `float` | alpha parameter. Used to relax the simulation (_see Parameters_)          |
| `-g <value>` | `float` | gamma parameter. Used to sharpen the color transitions (_see Parameters_) |
| `-e`         |         | Display equipotential lines instead of the absolute potential             |
| `-n <value>` | `int`   | Number of lines between 0V and the maximum absolute potential             |
| `-i <value>` | `int`   | Number of iterations per frame                                            |
| `-w <value>` | `int`   | Width of the window                                                       |
| `-h <value>` | `int`   | Height of the window                                                      |
| `-f`         |         | Display fullscreen                                                        |

### File Format
The file given in `[FILE]` is a text file containing a list of coordinates with voltages. Each line contains a triplet of float values. The first value is the potential assigned to a vertex, the remaining two the $xy$-position of said vertex.
```
<voltage> <x> <y>
...
```
The lines are read in blocks of three. Each block represents one triangle. (The vertices of one triangle dont necessarily need to be on the same voltage -- in general, the triangle area is interpolated using Gouraud shading.) _The triangles must not overlap_.

The potential can be any value, since all averaging operations stay invariant under normalization. The coordinates seen on the screen range from `(-10, -25)` to `(40, 25)`.

Example file
```
A.txt:
100 -2  2
100  2 -2
100 -2 -2
100  2  2
100 -2  2
100  2 -2
```
### Parameters
##### Relaxation
To relax the simulation, the values can be smoothed out over time. This enables a more stable but also slower computation. This is done with the alpha parameter, ranging from 0 to 1. Instead of replacing every old value with the computed new value, both are weighted together using the following formula
![](https://raw.githubusercontent.com/nodatapoints/voltage-simulation/master/doc/img/formula0.png)
##### Shading
There are two modes of shading available: _normal_ and _equipotential_. Before shading, the potential is normalized to [-1;1]. This is done by dividing by the maximum absolute potential known to be present.

In normal mode, -1 to 1 is mapped onto blue to yellow. The higher the parameter gamma, the sharper the transition of color around 0V. This way, gradients around 0 are displayed in more detail than the steep gradients near the potential sources. The color map is a onedimensional gradient from 0 (blue) to 1 (yellow). The color at a given potential is computed via
![](https://raw.githubusercontent.com/nodatapoints/voltage-simulation/master/doc/img/formula1.png)

In equipotential mode, only certain evenly distributed voltages in the spectrum are highlighted. The parameter n sets the number of lines, while gamma now sets the 'sharpness'. The greater gamma, the thinner the lines. This can look good on small gradients, but looks pixelated in steep ones. Following formula is used
![](https://raw.githubusercontent.com/nodatapoints/voltage-simulation/master/doc/img/formula2.png)
