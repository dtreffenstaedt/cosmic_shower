#ifndef PARTICLEDISTRIBUTOR_H
#define PARTICLEDISTRIBUTOR_H

#include <atomic>
#include <fstream>
#include <future>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <condition_variable>

namespace Node {

class ParticleScorer;
class ClusterRule;
class Cluster;
struct PrimaryParticle;
class CoreRunner;

class ParticleDistributor {
public:
    explicit ParticleDistributor(CoreRunner* runner, const std::shared_ptr<ParticleScorer>& scorer, const std::string& directory, std::string config);
    auto distribute() -> void;
    auto collect(const std::string& secondaries) -> void;
    virtual ~ParticleDistributor();

private:
    [[nodiscard]] auto has_next() const -> bool;
    [[nodiscard]] auto next() -> PrimaryParticle;
    auto parse(const std::string& secondaries) -> void;
    auto pre_conditions() -> void;
    auto run() -> std::future<void>;

    CoreRunner* m_runner;
    std::string m_directory { "./primaries/" };
    std::string m_config_file { "shower.cfg" };
    std::shared_ptr<ParticleScorer> m_scorer { std::make_shared<ParticleScorer>() };
    std::vector<PrimaryParticle> m_primaries {};
    std::shared_ptr<ClusterRule> m_cluster_rule {};
    std::condition_variable m_has_primaries {};
    std::atomic<bool> m_run { true };
    std::mutex m_primary_mutex {};
    std::future<void> m_future {};
};

}
#endif // PARTICLEDISTRIBUTOR_H
