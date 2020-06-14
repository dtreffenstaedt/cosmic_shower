# Cosmic Shower
Cosmic shower simulation in Geant4

### building
The simulation uses CMake as buildsystem.

Dependencies are: 
- Geant4 >= 10.6
- libconfig++ >= 1.7.2

To build, execute the `generate` script from the build directory and run `make`. This script sets the compiler to clang++. If you prefer to use gcc, you can also just execute `cmake ../source` from the build directory.

Default compiler options are
```
--std=c++17 -Wall -Werror -Wextra -Wpedantic -O3
```


### cosmic_shower

This is the simulation software, it reads the configuration file `shower.cfg` from the directory it is executed from.

The simulation runs headless per default. It reads the configuration file and starts the run, while creating a data output directory as set in the configuration file by the `data_directory` and `name` preferences. In this directory, each detector creates its own file where it records the individual hits in a CSV format.
The columns hold values as follows:
```
Particle,position_x,position_y,position_z,momentum_x,momentum_y,momentum_z,momentum_magnitude,global_time,local_time,proper_time 
```
if there are multiple runs in one simulation, each run will be separated by a `# Event <n>` comment in the individual detector files.

Additionally to the hits, the actually used configuration is dumped to the file `config_dump.cfg` where all effective options of the run are recorded. If a number of deters has been defined in the configuration, the config dump contains all actually used positions.

Following commandline arguments are possible:
```
	-h		print this help
	-c <filename>	use the config file <filename>
		default: shower.cfg
	-ui		show the graphical user interface
```

### configuration

The configuration file is in the syntax of [libconfig](http://hyperrealm.com/libconfig/libconfig_manual.html#Configuration-Files).

You can set a different config file, however the default file is always available as fallback. That means that in the individual config file you only have to set the relevant parameters and can rely on the fallback providing sane default values for the rest of the configuration.

The only required field is the name property, since that will be used as a folder name to store the resulting data from the run.

A default configuration file can be found in `source/config/shower.cfg.in` This file is automatically moved to the build directory by CMake. So if you need to make adjustments to the default configuation, do it in the .in file.

### atmospheric_layers

This program iteratively calculates the individual layer thickness according to a specific metric.
The default is set to layer mass. The change is made in the line which reads
```
Layer* layers = Layer::create(lower, upper, new Density(), n);
```
There are two more targets available, one is `Pressure` and one is `Temperature`.
In case you need a different metric, inherit `OptimisationTarget` and pass the new child class to the create method.
The optimisation method tries to keep the integral defined by the `integral` method of each individual layer equal to the average of all layers.

It takes the following arguments:
```
	-h		print this help
	-u <double>	set upper limit unit: m
		default: 40000 m
	-T <double>	Set temperature at sealevel unit: K
		default: 288.15 K
	-p <double>	Set pressure at sealevel unit: Pa
		default: 101325 Pa
	-rho <double>	Set density at sealevel unit: kg/m^3
		default: 1.225 kg/m^3
	-k <double>	Set heat capacity ratio
		default: 1.235
	-n <int>	set number of layers
		default: 10
	-csv		generate csv formatted output
	-config		generate libconfig formatted output
```
The parameter `-k` is the heat capacit ratio. it is dependent on the temperature gradient of the atmosphere and can be calculated with the formula
```
κ = (M * g) / (M * g + R * dT/dh)
```
Due to the default temperature gradient being linear, there is a cutoff altitude which cannot be crossed.
With the current default values this cutoff altitude is roughly 44300 m.
