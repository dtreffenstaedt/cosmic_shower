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

struct MagneticField
{
    double x; // x component in microTesla
    double y; // y component in microTesla
    double z; // z component in microTesla
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

    int n_particles;
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

class NoNameDefined : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: No name defined.";
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

class FaultyMagneticFieldDefinition : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Config error: Faulty magnetic field definition.";
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
    std::variant<std::vector<Config::DetectorPlacement>, size_t> get_detectors(const bool& fallback = false) const;

    /**
     * List of particles to be simulated
     */
    std::vector<Config::SecondaryParticle> get_particles(const bool& fallback = false) const;

    /**
     * List of atmospheric layers to be created
     */
    std::vector<Config::AtmosphereLayer> get_atmosphere_layers(const bool& fallback = false) const;

    /**
     * Primary particle definition
     */
    Config::PrimaryParticle get_primary_particle(const bool& fallback = false) const;

    /**
     * Magnetic field definition
     */
    Config::MagneticField get_magnetic_field(const bool& fallback = false) const;

    std::string get_name() const;

    std::string get_data_directory(const bool& fallback = false) const;

    double get_world_size(const bool& fallback = false) const;
    double get_atmosphere_height(const bool& fallback = false) const;
private:
    const libconfig::Setting& get_root(const bool& fallback = false) const;

    libconfig::Config m_config;
    libconfig::Config m_fallback;

    static ConfigManager* c_singleton;
};
}

#endif // CONFIGMANAGER_H
