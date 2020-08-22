#include "clusterrule.h"

#include <fstream>
#include <iterator>

#include "cluster.h"

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

SimpleClusterRule::SimpleClusterRule(std::shared_ptr<ParticleScorer> scorer)
    : ClusterRule(scorer)
{
}

auto SimpleClusterRule::result(const PrimaryParticle& /*particle*/) const -> Result
{
    if (current_cluster()->number() >= s_max) {
        return Result::NewCluster;
    }
    return Result::SameCluster;
}

FixedClusterRule::FixedClusterRule(std::shared_ptr<ParticleScorer> scorer, const std::string& secondaries)
    : ClusterRule(scorer)
{
    std::ifstream myfile(secondaries);

    myfile.unsetf(std::ios_base::skipws);

    std::size_t lines = static_cast<std::size_t>(std::count(
        std::istream_iterator<char>(myfile),
        std::istream_iterator<char>(),
        '\n'));

    m_max = (lines / s_number) + 1;
}

auto FixedClusterRule::result(const PrimaryParticle& /*particle*/) const -> Result
{
    if (current_cluster()->number() >= m_max) {
        return Result::NewCluster;
    }
    return Result::SameCluster;
}
}
