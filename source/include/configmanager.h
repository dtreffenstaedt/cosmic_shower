#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "global.h"
#include <libconfig.h++>
#include <string>
#include <vector>
#include <variant>

START_NAMESPACE
{
struct DetectorPlacement
{
    double x;
    double y;
    double z;

    std::string name;
};

struct InitialParticle
{
    double x;   // X position
    double y;   // Y position
    double z;   // Z position

    double m_x; // x momentum
    double m_y; // y momentum
    double m_z; // z momentum

    double m;   // momentum magnitude

    std::string type;   // type of initial particle
};

struct AtmosphereLayer
{
    double lower;   // lower bound of layer
    double upper;   // upper bound of layer
    double density; // average density of layer
    double pressure;    // average pressure of layer
    double temperature; // average temperature of layer
};


class NoDetectorsDefined : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: No detectors defined.";
    }
};

class FaultyDetectorDefinition : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: Faulty detector definition.";
    }
};

class FaultyAtmosphereLayerDefinition : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: Faulty atmosphere layer definition.";
    }
};

class ConfigManager
{
public:
    ConfigManager(const std::string& fileName);
    virtual ~ConfigManager();

    static ConfigManager* singleton();

    /**
     * Returns either a list of Detectors or the number of detectors to generate.
     */
    std::variant<std::vector<DetectorPlacement>, size_t> get_detectors() const;
    
    /**
     * List of particles to be simulated
     */
    std::vector<std::string> get_particles() const;
    
    /**
     * List of atmospheric layers to be created
     */
    std::vector<AtmosphereLayer> get_atmosphere_layers() const;

    /**
     * Initial particle definition
     */
    InitialParticle get_initial_particle() const;
private:

    libconfig::Config m_config;

    static ConfigManager* c_singleton;
};
}

#endif // CONFIGMANAGER_H
