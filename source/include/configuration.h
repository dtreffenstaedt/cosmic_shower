#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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
        struct
        {
            double z;
            double a;
            double rho;
        } physical;
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
            bool absolute;
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
        std::string name;
    };

    struct AtmosphereLayer {
        int id;
        double lower; // lower bound of layer
        double upper; // upper bound of layer
        double density; // average density of layer
        double pressure; // average pressure of layer
        double temperature; // average temperature of layer
    };
}

class NoNameDefined : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class ArgumentError : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class FaultyDetectorDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class FaultyAtmosphereLayerDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class FaultyPrimaryDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class FaultySecondaryDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class FaultyMagneticFieldDefinition : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class Configuration {
public:
    explicit Configuration(std::string file_name = "shower.cfg");

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
    [[nodiscard]] auto get_primaries(const bool& fallback = false) const -> std::vector<Config::PrimaryParticle>;

    //    [[nodiscard]] auto get_initial_ratio(const bool& fallback = false) const -> double;

    [[nodiscard]] auto get_primary_event_limit(const bool& fallback = false) const -> double;

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

    std::string m_file_name;
};
}

#endif // CONFIGURATION_H
