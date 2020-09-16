#ifndef PARTICLESCORER_H
#define PARTICLESCORER_H

#include <string>

namespace Node {

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
    struct {
        double global;
    } time;

    int n_particles;
    int particle; // type of initial particle
    std::string name;
};

class ParticleScorer {
public:
    [[nodiscard]] virtual auto score(const PrimaryParticle& particle) const -> double;

    [[nodiscard]] constexpr auto limit() const -> double {
        return s_lower_limit;
    }

    virtual ~ParticleScorer() = default;
private:
    static constexpr double s_lower_limit { 1.0e-1 };
};

}

#endif // PARTICLESCORER_H
