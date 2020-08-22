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

    int n_particles;
    int particle; // type of initial particle
    std::string name;
};

class ParticleScorer {
public:
    [[nodiscard]] virtual auto score(const PrimaryParticle& particle) const -> double;

    virtual ~ParticleScorer() = default;
};

}

#endif // PARTICLESCORER_H
