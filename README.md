# Cosmic Shower
Cosmic shower simulation in Geant4

### building
Dependencies are: 
- Geant4 >= 10.6
- libconfig(++) >= 1.7.2

To build, execute the `generate` script from the build directory and run `make`.


### cosmic_shower

This is the simulation software, it reads the configuration file `shower.cfg` from the directory it is executed from.

It does not produce usable output as of yet.

### configuration

The configuration file is in the syntax of [libconfig](http://hyperrealm.com/libconfig/libconfig_manual.html#Configuration-Files).

More to come. 

### atmospheric_layers

This program iteratively calculates the individual layer thickness so the mass of all layers is the same. 

It takes the following arguments:
```
possible parameters:
	-h		print this help
	-u <double>	set upper limit
	-l <double>	set lower limit
	-n <int>	set number of layers
	--csv		generate csv formatted output
```
execution may take a while depending on number of layers and total thickness.
