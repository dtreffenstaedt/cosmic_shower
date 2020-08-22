#include "particlescorer.h"

namespace Node {

auto ParticleScorer::score(const PrimaryParticle& particle) const -> double
{
    double score { 0 };
    score += particle.momentum.m * -0.1;

    return score;
}

}
