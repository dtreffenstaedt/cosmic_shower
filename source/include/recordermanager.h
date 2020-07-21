#ifndef RECORDERMANAGER_H
#define RECORDERMANAGER_H

#include "configmanager.h"
#include "global.h"

#include <array>
#include <atomic>
#include <queue>

#include <G4ThreeVector.hh>

namespace Shower {

class RecorderManager {
public:
    RecorderManager();
    virtual ~RecorderManager();

    [[nodiscard]] auto stored_primary() const -> bool;

    void store_primary(const G4ThreeVector& vector, const G4double& time);
    void store_hit(const G4ThreeVector& vector, const G4double& time);

    void store_momentum(const G4ThreeVector& vector, const G4double& momentum);

    static auto singleton() -> RecorderManager*;

    template <size_t N>
    class Bin {
    public:
        void initialise(const size_t& x, const size_t& y, const double& world_size);

        auto in_bin(const double& x, const double& y) const -> bool;

        void store_momentum(const double& momentum);

        auto get_value() const -> double;

        auto get_x_center() const -> double;
        auto get_y_center() const -> double;

    private:
        double m_value { 0 };
        double m_x_min { 0 };
        double m_x_max { 0 };
        double m_y_min { 0 };
        double m_y_max { 0 };
        double m_inverse_bin_area { 0 };
    };

private:
    struct Hit {
        G4ThreeVector pos;
        G4double time;
    };
    void save();

    static RecorderManager* c_singleton;

    std::atomic<bool> m_stored_primary { false };

    Hit m_primary {};

    std::queue<Hit> m_hits {};
    static constexpr size_t m_size { 10 };

    std::array<std::array<Bin<m_size>, m_size>, m_size> m_bins {};
};
}

#endif // RECORDERMANAGER_H
