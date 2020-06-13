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

Following arguments are possible:
```
	-h		print this help
	-c <filename>	use the config file <filename>
		default: shower.cfg
```

### configuration

The configuration file is in the syntax of [libconfig](http://hyperrealm.com/libconfig/libconfig_manual.html#Configuration-Files).

The default configuration is as follows:

```
# detectors to simulate either assign a number to get a number of randomly generated detectors or use a list.
detectors = 1; # number of detectors to generate, they will be spread uniformly across the surface of earth.
# example for list of detectors: the name is an arbitrary identifier. the values for x,y and z are double values in meters.
# x=0,y=0 is in the center of the simulated area, z=0 corresponds to the surface of earth.
#detectors = (
#    {
#        x = 0.0;
#        y = 0.0;
#        z = 0.0;
#        name = "detector1";
#    },
#    {
#        x = 100.0;
#        y = 100.0;
#        z = 0.0;
#        name = "detector2";
#    });

# options for the primary particle.
# origin refers to the position where the particle is generated.
# x=0,y=0 refers to the middle of the simulated area, z = 0 corresponds to the top of the atmosphere. The values are doubles in meters.
# the momentum consists of a unit vector and a magnitude, the magnitude is a double value in MeV.
# the parameter particle refers to the PDG code of the primary particle.
primary = {
    origin = {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    };

    momentum = {
        x = 0.0;
        y = 0.0;
        z = -1.0;
        magnitude = 1000.0;
    };

    particle = 2212;
};

# list of particles to simulate, use PDG codes.
particles = [11, 13, 111, 211];

# default atmospheric layer configuration. Do NOT edit unless you absolutely know what you're doing.
# If you want to change atmospheric parameters, use the atmosphere_layer executable to generate a new configuration.
layers = ( 
  {
    id = 0;
    lower = 0.0;
    upper = 879.659832;
    density = 1.174369;
    pressure = 96186.753628;
    temperature = 285.290309;
  }, 
  {
    id = 1;
    lower = 879.659832;
    upper = 1842.382146;
    density = 1.073046;
    pressure = 86045.815026;
    temperature = 279.3009;
  }, 
  {
    id = 2;
    lower = 1842.382146;
    upper = 2908.044947;
    density = 0.969392;
    pressure = 75902.605919;
    temperature = 272.706812;
  }, 
  {
    id = 3;
    lower = 2908.044947;
    upper = 4105.056996;
    density = 0.86302;
    pressure = 65756.072928;
    temperature = 265.351071;
  }, 
  {
    id = 4;
    lower = 4105.056996;
    upper = 5476.235372;
    density = 0.753399;
    pressure = 55604.390176;
    temperature = 257.002128;
  }, 
  {
    id = 5;
    lower = 5476.235372;
    upper = 7090.953781;
    density = 0.639768;
    pressure = 45444.089333;
    temperature = 247.295261;
  }, 
  {
    id = 6;
    lower = 7090.953781;
    upper = 9074.026649;
    density = 0.520931;
    pressure = 35267.66402;
    temperature = 235.599183;
  }, 
  {
    id = 7;
    lower = 9074.026649;
    upper = 11690.94568;
    density = 0.394756;
    pressure = 25055.19091;
    temperature = 220.645046;
  }, 
  {
    id = 8;
    lower = 11690.94568;
    upper = 15722.476219;
    density = 0.256241;
    pressure = 14729.16586;
    temperature = 199.031568;
  }, 
  {
    id = 9;
    lower = 15722.476219;
    upper = 40000.0;
    density = 0.042551;
    pressure = 1908.020616;
    temperature = 107.001519;
  } );
```

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
execution may take a while depending on number of layers and total thickness.
