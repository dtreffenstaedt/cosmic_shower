#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "global.h"
#include <libconfig.h++>
#include <string>
#include <variant>
#include <vector>

namespace Shower {
namespace Config {
    struct DetectorProperties {
        struct
        {
            double x;
            double y;
            double z;
        } geometry;
    };
    struct TrackingCuts {
        double energy;
        double range;
    };

    struct DetectorPlacement {
        double x {};
        double y {};
        double z {};

        std::string name;
    };

    struct MagneticField {
        double x; // x component in microTesla
        double y; // y component in microTesla
        double z; // z component in microTesla
    };

    struct SecondaryParticle {
        int pdg; // PDG code
    };

    struct PrimaryParticle {
        struct
        {
            double x; // X position
            double y; // Y position
            double z; // Z position
        } origin;

        struct
        {
            double x; // x momentum
            double y; // y momentum
            double z; // z momentum

            double m; // momentum magnitude
        } momentum;

        int n_particles;
        int particle; // type of initial particle
    };

    struct AtmosphereLayer {
        double lower; // lower bound of layer
        double upper; // upper bound of layer
        double density; // average density of layer
        double pressure; // average pressure of layer
        double temperature; // average temperature of layer
    };
}

class NoNameDefined : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Config error: No name defined.";
    }
};

class ArgumentError : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "commandline argument error.";
    }
};

class FaultyDetectorDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Config error: Faulty detector definition.";
    }
};

class FaultyAtmosphereLayerDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Config error: Faulty atmosphere layer definition.";
    }
};

class FaultyPrimaryDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Config error: Faulty primary definition.";
    }
};

class FaultySecondaryDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Config error: Faulty secondary definition.";
    }
};

class FaultyMagneticFieldDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Config error: Faulty magnetic field definition.";
    }
};

class ConfigManager {
public:
    explicit ConfigManager(std::string file_name = "shower.cfg");
    virtual ~ConfigManager();

    static auto singleton() -> ConfigManager*;

    /**
     * Returns either a list of Detectors or the number of detectors to generate.
     */
    [[nodiscard]] auto get_detectors(const bool& fallback = false) const -> std::variant<std::vector<Config::DetectorPlacement>, size_t>;

    /**
     * List of particles to be simulated
     */
    [[nodiscard]] auto get_particles(const bool& fallback = false) const -> std::vector<Config::SecondaryParticle>;

    /**
     * List of atmospheric layers to be created
     */
    [[nodiscard]] auto get_atmosphere_layers(const bool& fallback = false) const -> std::vector<Config::AtmosphereLayer>;

    /**
     * Primary particle definition
     */
    [[nodiscard]] auto get_primary_particle(const bool& fallback = false) const -> Config::PrimaryParticle;

    [[nodiscard]] auto get_initial_ratio(const bool& fallback = false) const -> double;

    [[nodiscard]] auto get_detector_properties(const bool& fallback = false) const -> Config::DetectorProperties;

    /**
     * Magnetic field definition
     */
    [[nodiscard]] auto get_magnetic_field(const bool& fallback = false) const -> Config::MagneticField;

    [[nodiscard]] auto get_tracking_cut(const bool& fallback = false) const -> Config::TrackingCuts;

    [[nodiscard]] auto get_name() const -> std::string;

    [[nodiscard]] auto get_data_directory(const bool& fallback = false) const -> std::string;

    [[nodiscard]] auto get_events(const bool& fallback = false) const -> int;

    [[nodiscard]] auto get_world_size(const bool& fallback = false) const -> double;
    [[nodiscard]] auto get_atmosphere_height(const bool& fallback = false) const -> double;

    void config_dump(const std::string& filename);

    void add_detector(const Config::DetectorPlacement& detector);

private:
    [[nodiscard]] auto get_root(const bool& fallback = false) const -> const libconfig::Setting&;

    libconfig::Config m_config;
    libconfig::Config m_fallback;

    std::vector<Config::DetectorPlacement> m_detectors;

    static ConfigManager* c_singleton;

    std::string m_file_name;
};
}

#endif // CONFIGMANAGER_H
