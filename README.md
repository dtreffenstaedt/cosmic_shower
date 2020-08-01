# Cosmic Shower
Cosmic shower simulation in Geant4

### building
The simulation uses CMake as buildsystem.

Dependencies are: 
- Geant4 >= 10.6
- libconfig++ >= 1.7.2

To build, run `cmake ../source/`, then `make` from the build directory.

If you want to use a graphical user interface, run cmake with `-DSHOWER_BUILD_UI=ON`. Default is OFF.

If you want to perform benchmarks, run cmake with `-DSHOWER_BENCHMARK=ON`. Default is OFF.

This saves the execution time per event and measures memory usage over the program runtime. Note that this option reduces performance.

Default compiler options are
```
-Weverything -Wno-c++98-compat -Wno-padded -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wall -Wextra -Wpedantic -Wshadow -Werror -O3
```

There are two executables generated, one is `cosmic_shower`, which is the simulation itself.

The other is `atmosphere_layers` which calculates the optimal atmosphere layer thickness.

All binaries will be placed in the `build/bin/` directory.

### cosmic_shower

This is the simulation software, it reads the configuration file `shower.cfg` from the directory it is executed from.

The simulation runs headless per default. It reads the configuration file and starts the run, while creating a data output directory as set in the configuration file by the `data_directory` and `name` preferences. In this directory all output files are placed.
These output files are: 
```
config_dump		contains the used configuration
detailed_hits		contains all detailed hit information from the detectors
hits			contains a list of hits on detectors with only position and timestamp to test multilateration easily
ground_intensity	contains the amount of energy arrived at ground level separated into chunks
secondaries		If the simulation was cut short this contains the list of all particles active in the simulation at time of shutdown
```
The Data in the `secondaries` file is formatted in the libconfig format so it can be directly used to restart the simulation with those particles

The `detailed_hits` file is a csv file with the follwing columns
```
Particle,position_x,position_y,position_z,momentum_x,momentum_y,momentum_z,momentum_magnitude,global_time,local_time,proper_time 
```
if there are multiple runs in one simulation, each run will be separated by a `# Event <n>` comment.

Following commandline arguments are possible:
```
	-h	--help
			Print this help
	-g	--graphical
			Show the user interface
	-o	--overwrite
			Overwrite existing simulation data
	-c	--config (value)
			Use the configuration file specified in the value
```
The ´-g´ option is only avialable if `-DSHOWER_BUILD_UI` has been set to `ON`.

### configuration

The configuration file is in the syntax of [libconfig](http://hyperrealm.com/libconfig/libconfig_manual.html#Configuration-Files).

You can set a different config file, however the default file is always available as fallback. That means that in the individual config file you only have to set the relevant parameters and can rely on the fallback providing sane default values for the rest of the configuration.

The only required field is the name property, since that will be used as a folder name to store the resulting data from the run.

A default configuration file can be found in `source/config/shower.cfg.in` This file is automatically moved to the build directory by CMake. So if you need to make adjustments to the default configuation, do it in the .in file.

### atmosphere_layers

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
