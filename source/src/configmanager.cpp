#include "configmanager.h"

#include <iostream>

START_NAMESPACE
{
ConfigManager* ConfigManager::c_singleton = 0;

ConfigManager* ConfigManager::singleton()
{
    return c_singleton;
}

ConfigManager::ConfigManager(const std::string& fileName)
{
    m_fallback.readFile("shower.cfg");

    m_config.readFile(fileName.c_str());

    if (!get_root().exists("name"))
    {
        throw NoNameDefined();
    }

    c_singleton = this;
}

ConfigManager::~ConfigManager()
{}

std::variant<std::vector<Config::DetectorPlacement>, size_t> ConfigManager::get_detectors(const bool& fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("detectors"))
        {
            return get_detectors(true);
        }
    }
    unsigned int number;
    if (!get_root(fallback).lookupValue("detectors", number))
    {
        const libconfig::Setting& detectors_setting = get_root(fallback)["detectors"];
        std::vector<Config::DetectorPlacement> detectors;
        size_t len = detectors_setting.getLength();
        for (size_t i = 0; i < len; i++)
        {
            const libconfig::Setting& detector = detectors_setting[i];
            Config::DetectorPlacement settings;
            if (!(detector.lookupValue("x", settings.x) &&
                detector.lookupValue("y", settings.y) &&
                detector.lookupValue("z", settings.z) &&
                detector.lookupValue("name", settings.name))
                )
            {
                throw FaultyDetectorDefinition();
            }
            detectors.push_back(settings);
        }
        return detectors;
    }
    return static_cast<size_t>(number);
}

std::vector<Config::AtmosphereLayer> ConfigManager::get_atmosphere_layers(const bool& fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("layers"))
        {
            return get_atmosphere_layers(true);
        }
    }
    const libconfig::Setting& layers_setting = get_root(fallback)["layers"];
    std::vector<Config::AtmosphereLayer> layers;
    size_t len = layers_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        const libconfig::Setting& layer = layers_setting[i];
        Config::AtmosphereLayer settings;
        if (!(layer.lookupValue("lower", settings.lower) &&
              layer.lookupValue("upper", settings.upper) &&
              layer.lookupValue("density", settings.density) &&
              layer.lookupValue("pressure", settings.pressure) &&
              layer.lookupValue("temperature", settings.temperature))
            )
        {
            throw FaultyAtmosphereLayerDefinition();
        }
        layers.push_back(settings);
    }
    return layers;
}

std::vector<Config::SecondaryParticle> ConfigManager::get_particles(const bool& fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("particles"))
        {
            return get_particles(true);
        }
    }
    const libconfig::Setting& particles_setting = get_root(fallback)["particles"];
    std::vector<Config::SecondaryParticle> particles;
    size_t len = particles_setting.getLength();
    for (size_t i = 0; i < len; i++)
    {
        const libconfig::Setting& particle = particles_setting[i];
        Config::SecondaryParticle settings;
        if (!(particle.lookupValue("pdg", settings.pdg)))
        {
            throw FaultySecondaryDefinition();
        }
        if (particle.exists("cut_range"))
        {
            if (!particle.lookupValue("cut_range", settings.cut_range))
            {
                throw FaultySecondaryDefinition();
            }
        }
        else
        {
            settings.cut_range = 0;
        }
        if (particle.exists("cut_range"))
        {
            if (!particle.lookupValue("cut_energy", settings.cut_energy))
            {
                throw FaultySecondaryDefinition();
            }
        }
        else
        {
            settings.cut_energy = 0;
        }
        particles.push_back(settings);
    }
    return particles;
}

Config::PrimaryParticle ConfigManager::get_primary_particle(const bool& fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("primary"))
        {
            return get_primary_particle(true);
        }
    }
    const libconfig::Setting& primary_setting = get_root(fallback)["primary"];
    Config::PrimaryParticle primary;
    const libconfig::Setting& origin = primary_setting["origin"];
    const libconfig::Setting& momentum = primary_setting["momentum"];

    if (!(origin.lookupValue("x", primary.origin.x) &&
        origin.lookupValue("y", primary.origin.y) &&
        origin.lookupValue("z", primary.origin.z) &&
        momentum.lookupValue("x", primary.momentum.y) &&
        momentum.lookupValue("y", primary.momentum.y) &&
        momentum.lookupValue("z", primary.momentum.z) &&
        momentum.lookupValue("magnitude", primary.momentum.m) &&
        primary_setting.lookupValue("n_particles", primary.n_particles) &&
        primary_setting.lookupValue("particle", primary.particle)))
    {
        throw FaultyPrimaryDefinition();
    }
    return primary;
}

Config::DetectorProperties ConfigManager::get_detector_properties(const bool &fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("detector_properties"))
        {
            return get_detector_properties(true);
        }
    }
    const libconfig::Setting& detector_setting = get_root(fallback)["detector_properties"];
    Config::DetectorProperties detector;
    const libconfig::Setting& geometry = detector_setting["geometry"];

    if (!(geometry.lookupValue("x", detector.geometry.x) &&
        geometry.lookupValue("y", detector.geometry.y) &&
        geometry.lookupValue("z", detector.geometry.z)))
    {
        throw FaultyDetectorDefinition();
    }
    return detector;
}

Config::MagneticField ConfigManager::get_magnetic_field(const bool& fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("magnetic_field"))
        {
            return get_magnetic_field(true);
        }
    }
    const libconfig::Setting& field_setting = get_root(fallback)["magnetic_field"];
    Config::MagneticField field;

    if (!(field_setting.lookupValue("x", field.x) &&
        field_setting.lookupValue("y", field.y) &&
        field_setting.lookupValue("z", field.z)))
    {
        throw FaultyMagneticFieldDefinition();
    }
    return field;
}

std::string ConfigManager::get_name() const
{
    return m_config.lookup("name");
}

std::string ConfigManager::get_data_directory(const bool &fallback) const
{
    if (fallback || !m_config.exists("data_directory"))
    {
        return m_fallback.lookup("data_directory");
    }
    return m_config.lookup("data_directory");
}

const libconfig::Setting& ConfigManager::get_root(const bool& fallback) const
{
    if (fallback)
    {
        return m_fallback.getRoot();
    }
    return m_config.getRoot();
}

double ConfigManager::get_world_size(const bool& fallback) const
{
    double size = 0;
    if (!fallback)
    {
        if (!get_root().exists("world_size"))
        {
            return get_world_size(true);
        }
    }
    if (!get_root(fallback).lookupValue("world_size", size))
    {
        throw FaultyAtmosphereLayerDefinition();
    }
    return size;
}

double ConfigManager::get_atmosphere_height(const bool &fallback) const
{
    if (!fallback)
    {
        if (!get_root().exists("layers"))
        {
            return get_atmosphere_height(true);
        }
    }
    const libconfig::Setting& layers_setting = get_root(fallback)["layers"];
    double upper;
    size_t len = layers_setting.getLength();
    if (!(layers_setting[len - 1].lookupValue("upper", upper)))
    {
        throw FaultyAtmosphereLayerDefinition();
    }
    return upper;
}
}
