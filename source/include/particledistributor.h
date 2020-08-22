#ifndef PARTICLEDISTRIBUTOR_H
#define PARTICLEDISTRIBUTOR_H

#include <fstream>
#include <future>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace Node {

class ParticleScorer;
class ClusterRule;
class Cluster;
struct PrimaryParticle;

class ParticleDistributor {
public:
    explicit ParticleDistributor(const std::string& directory, std::string config, const std::string& secondaries);
    auto distribute() -> void;
    virtual ~ParticleDistributor();

private:
    [[nodiscard]] auto has_next() const -> bool;
    [[nodiscard]] auto next() -> PrimaryParticle;
    auto parse() -> void;

    std::vector<std::shared_ptr<Cluster>> m_clusters {};

    std::string m_directory { "./primaries/" };
    std::string m_config_file { "shower.cfg" };
    std::string m_secondaries { "secondaries" };
    std::shared_ptr<ParticleScorer> m_scorer { std::make_shared<ParticleScorer>() };
    std::shared_ptr<ClusterRule> m_cluster_rule {};
    std::queue<PrimaryParticle> m_primaries {};
    std::ifstream m_stream {};
    bool m_end { false };
    std::vector<std::future<void>> m_threads {};
};
}
#endif // PARTICLEDISTRIBUTOR_H
