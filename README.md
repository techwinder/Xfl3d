# Xfl3d
Fun scientific animations using OpenGL shaders

# Description
Xfl3d provides a set of 2d and 3d animations based on OpenGL and the Qt framework. These animations were implemented during the development of flow5 to experiment with OpenGL features.

## Mandelbrot and Julia Sets
![image info](./meta/images/Mandelbrot.png)

![image info](./meta/images/Julia.png)

## Newton fractal
![image info](./meta/images/Newton.png)

## Quaternion Julia set
![image info](./meta/images/Quat_Julia.png)

## Shadow
![image info](./meta/images/Shadow.png)

## Hydrogen atom
![image info](./meta/images/Hydrogen.png)

## Flight
![image info](./meta/images/Flight.png)

## Lorenz attractor - CPU based
![image info](./meta/images/Lorenz_CPU.png)

## Lorenz attractor - GPU based
![image info](./meta/images/Lorenz_GPU.png)

## 3d attractors
![image info](./meta/images/attractors.png)

## Solar system
![image info](./meta/images/Solarsys.png)

## Sagittarius A*
![image info](./meta/images/Sagittarius.png)

## NEARGALCAT (the final frontier)
![image info](./meta/images/NEARGALCAT.png)

## PSO and GA optimizations
![image info](./meta/images/Optim2d.png)

## Boids - CPU based
![image info](./meta/images/Boids_CPU.png)

## Boids - GPU based
![image info](./meta/images/Boids_GPU.png)

## Vortex flow5
![image info](./meta/images/Vortex_flow.png)

# Usage
## Mouse
Pan:    left mouse button

Zoom:   mouse wheel, or Alt+ Mouse move vertically or laterally

Rotate: Ctrl or mouse middle button to activate the arcball, then click on the arcball and mouse move

## Keyboar shortcuts
R:         Reset the scale

I:         Switch to pseudo-isometric view

X:         Front x-view

Shift + X: Back x-view

Y:         Front y-view

Shift + Y: Back y-view

Z:         Front z-view

Shift + Z: Back z-view

H:         Flip horizontally

V:         Flip vertically

# Compilation
## Linux
- Use your distribution's pacakge manager to  install the Qt6()-dev and Mesa()-dev libraries
- `qmake`
- `make`
- `sudo make install`
- `Xfl3d` to run

## Windows
- Install the Qt6 development libraries from the Qt site
- Compile and run in release mode

## MAcOS
- Not available: Some of the animations require OpenGL 4.3+ whereas MacOS only provides OpenGL 4.1
