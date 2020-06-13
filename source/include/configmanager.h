#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "global.h"
#include <libconfig.h++>
#include <string>
#include <vector>
#include <variant>

START_NAMESPACE
{
namespace Config
{
struct DetectorPlacement
{
    double x;
    double y;
    double z;

    std::string name;
};

struct SecondaryParticle
{
    int pdg;    // PDG code
    double cut_range;   // Range Cut in mm
    double cut_energy; // Energy Cut in MeV
};

struct PrimaryParticle
{
    struct
    {
        double x;   // X position
        double y;   // Y position
        double z;   // Z position
    } origin;

    struct
    {
        double x; // x momentum
        double y; // y momentum
        double z; // z momentum

        double m;   // momentum magnitude
    } momentum;

    int particle;   // type of initial particle
};

struct AtmosphereLayer
{
    double lower;   // lower bound of layer
    double upper;   // upper bound of layer
    double density; // average density of layer
    double pressure;    // average pressure of layer
    double temperature; // average temperature of layer
};
}

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

class FaultyPrimaryDefinition : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: Faulty primary definition.";
    }
};

class FaultySecondaryDefinition : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: Faulty secondary definition.";
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
    std::variant<std::vector<Config::DetectorPlacement>, size_t> get_detectors() const;

    /**
     * List of particles to be simulated
     */
    std::vector<Config::SecondaryParticle> get_particles() const;

    /**
     * List of atmospheric layers to be created
     */
    std::vector<Config::AtmosphereLayer> get_atmosphere_layers() const;

    /**
     * Primary particle definition
     */
    Config::PrimaryParticle get_primary_particle() const;
private:

    libconfig::Config m_config;

    static ConfigManager* c_singleton;
};
}

#endif // CONFIGMANAGER_H
