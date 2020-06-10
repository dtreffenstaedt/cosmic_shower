# Cosmic Shower
Cosmic shower simulation in Geant4

### building
Dependencies are: 
- Geant4 >= 10.6
- libconfig(++) >= 1.7.2

To build, execute the generate script from the build directory and run make.


### atmospheric_layers

To minimise the error caused by a discrete number of layers in the atmospheric simulation, the layer thickness is non-uniform.

The atmospheric_layers program calculates the individual layer thickness. To do that, it iteratively adjusts the thicknesses until 

![eqn](https://latex.codecogs.com/gif.latex?%5Cint_%7Bh_%7Bmin_i%7D%7D%5E%7Bh_%7Bmax_i%7D%7D%7B%5Crho%28h%29dh%7D%20-%20%5Cfrac%7B1%7D%7BN%7D%5Cint_%7Bh_%7Bmin%7D%7D%5E%7Bh_%7Bmax%7D%7D%7B%5Crho%28h%29dh%7D%20%3D%200)

for every layer

where

h_max_i:	lower bound of layer

h_min_i:	upper bound of layer

h_min:		lower bound of simulated region

h_max:		upper bound of simulated region

ρ(h):		density by altitude

N:		number of layers

The program takes the following arguments:
```
possible parameters:
	-h		print this help
	-u <double>	set upper limit
	-l <double>	set lower limit
	-n <int>	set number of layers
	--csv		generate csv formatted output`
```
execution may take a while depending on number of layers and total thickness.

### cosmic_shower

This is the simulation software, it reads the configuration file `shower.cfg` from the directory it is executed from.

It does not produce usable output as of yet.

### configuration

The configuration file is in the syntax of [libconfig](http://hyperrealm.com/libconfig/libconfig_manual.html#Configuration-Files).

More to come. 

