#include "clusterrule.h"

#include <algorithm>
#include <fstream>
#include <iterator>

#include "cluster.h"
#include "particlescorer.h"

namespace Node {

ClusterRule::ClusterRule(std::shared_ptr<ParticleScorer> scorer)
    : m_scorer { std::move(scorer) }
{
}

auto ClusterRule::update_cluster(std::shared_ptr<Cluster> cluster) -> void
{
    m_current_cluster.reset();
    m_current_cluster = cluster;
}

auto ClusterRule::result(const PrimaryParticle& /*particle*/) const -> Result
{
    return Result::SameCluster;
}

auto ClusterRule::current_cluster() const -> std::shared_ptr<Cluster>
{
    return m_current_cluster;
}

auto ClusterRule::has_cluster() const -> bool {
    return m_current_cluster != nullptr;
}

SimpleClusterRule::SimpleClusterRule(std::shared_ptr<ParticleScorer> scorer)
    : ClusterRule(scorer)
{
}

auto SimpleClusterRule::result(const PrimaryParticle& /*particle*/) const -> Result
{
    if (!has_cluster()) {
        return Result::NewCluster;
    }
    if (current_cluster()->number() >= s_max) {
        return Result::NewCluster;
    }
    return Result::SameCluster;
}

ScoreClusterRule::ScoreClusterRule(std::shared_ptr<ParticleScorer> scorer)
    : ClusterRule(scorer)
{
}

auto ScoreClusterRule::result(const PrimaryParticle& particle) const -> Result
{
    if (!has_cluster()) {
        return Result::NewCluster;
    }
    if ((current_cluster()->score() + m_scorer->score(particle)) >= m_max_score) {
        return Result::NewCluster;
    }
    return Result::SameCluster;
}

FixedClusterRule::FixedClusterRule(std::shared_ptr<ParticleScorer> scorer, const std::vector<PrimaryParticle>& secondaries)
    : ClusterRule(scorer)
{


    m_max = (secondaries.size() / s_number) + 1;
}

auto FixedClusterRule::result(const PrimaryParticle& /*particle*/) const -> Result
{
    if (!has_cluster()) {
        return Result::NewCluster;
    }
    if (current_cluster()->number() >= m_max) {
        return Result::NewCluster;
    }
    return Result::SameCluster;
}
}
