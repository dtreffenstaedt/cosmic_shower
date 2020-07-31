#ifndef RECORDER_H
#define RECORDER_H

#include "configuration.h"
#include "global.h"
#include "parameters.h"

#include <array>
#include <atomic>
#include <queue>

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

class Recorder {
public:
    Recorder(const std::shared_ptr<Configuration>& config, const std::shared_ptr<Parameters>& params);
    virtual ~Recorder();

    [[nodiscard]] auto stored_primary() const -> bool;

    void store_primary(const Hit& primary);
    void store_hit(const Hit& hit);

    void store_detailed_hit(const DetailedHit& hit);

    void store_ground_intensity(const GroundIntensity& intensity);

    template <size_t N>
    class Bin {
    public:
        void initialise(const size_t& x, const size_t& y, const double& world_size);

        auto in_bin(const double& x, const double& y) const -> bool;

        void store_momentum(const double& momentum);
        void store_energy(const double& energy);
        void store_charged();
        void store_uncharged();

        auto get_x_center() const -> double;
        auto get_y_center() const -> double;

        void store(std::ofstream& stream);

    private:
        double m_momentum_density { 0 };
        double m_energy_density { 0 };
        size_t m_n_charged { 0 };
        size_t m_n_uncharged { 0 };
        double m_x_min { 0 };
        double m_x_max { 0 };
        double m_y_min { 0 };
        double m_y_max { 0 };
        double m_inverse_bin_area { 0 };
    };

private:
    void save();

    std::string m_directory {};

    std::atomic<bool> m_stored_primary { false };

    Hit m_primary {};

    std::queue<Hit> m_hits {};
    std::queue<DetailedHit> m_detailed_hits {};
    static constexpr size_t m_size { 10 };

    std::array<std::array<Bin<m_size>, m_size>, m_size> m_bins {};
};
}

#endif // RECORDER_H
