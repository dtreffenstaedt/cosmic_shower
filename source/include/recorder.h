#ifndef RECORDER_H
#define RECORDER_H

#include "configuration.h"
#include "global.h"
#include "parameters.h"

#include <array>
#include <atomic>
#include <exception>
#include <vector>

#include <G4ThreeVector.hh>

namespace Shower {

struct DetailedHit {
    const G4int pdg {};
    const G4double momentum {};
    const G4ThreeVector momentum_direction {};
    const G4ThreeVector position {};
    const G4double global_time {};
    const G4double proper_time {};
    const G4String name {};
};

struct Hit {
    G4ThreeVector position {};
    G4double time {};
};

struct GroundIntensity {
    const G4ThreeVector position {};
    const G4double rest_energy {};
    const G4double energy {};
    const bool charged {};
};

struct Secondary {
    struct {
        const double x{};
        const double y{};
        const double z{};
    } position;
    struct {
        const double x{};
        const double y{};
        const double z{};
    } energy;
    const double kinetic_energy {};
    const int pdg {};
};

class DataDirectoryExists : public std::exception {
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class Recorder {
public:
    Recorder(const std::shared_ptr<Configuration>& config, const std::shared_ptr<Parameters>& params);
    virtual ~Recorder();

    [[nodiscard]] auto stored_primary() const -> bool;

    void store_primary(const Hit& primary);
    void store_hit(const Hit& hit);

    void store_detailed_hit(const DetailedHit& hit);

    void store_ground_intensity(const GroundIntensity& intensity);

    void store_secondary(const Secondary& intensity);

    void next_event();

    void save();

    template <size_t N>
    class Bin {
    public:
        void initialise(const size_t& x, const size_t& y, const double& world_size);

        [[nodiscard]] auto in_bin(const double& x, const double& y) const -> bool;

        void store_momentum(const double& momentum);
        void store_energy(const double& energy);
        void store_charged();
        void store_uncharged();

        [[nodiscard]] auto get_x_center() const -> double;
        [[nodiscard]] auto get_y_center() const -> double;

        void store(std::ofstream& stream);

        [[nodiscard]] auto empty() const -> bool;

    private:
        void clear();
        std::atomic<bool> m_empty { true };
        std::atomic<double> m_momentum_density { 0 };
        std::atomic<double> m_energy_density { 0 };
        std::atomic<size_t> m_n_charged { 0 };
        std::atomic<size_t> m_n_uncharged { 0 };
        std::atomic<double> m_x_min { 0 };
        std::atomic<double> m_x_max { 0 };
        std::atomic<double> m_y_min { 0 };
        std::atomic<double> m_y_max { 0 };
        std::atomic<double> m_inverse_bin_area { 0 };
    };

private:
    [[nodiscard]] auto directory() const -> std::string;
    [[nodiscard]] auto bins_empty() const -> bool;

    std::string m_directory {};

    std::atomic<bool> m_stored_primary { false };

    Hit m_primary {};

    std::vector<Hit> m_hits {};
    std::vector<DetailedHit> m_detailed_hits {};
    std::vector<Secondary> m_secondaries {};
    static constexpr size_t m_size { 10 };

    std::array<std::array<Bin<m_size>, m_size>, m_size> m_bins {};

    std::atomic<size_t> m_event_number { 0 };
};
}
#endif // RECORDER_H
