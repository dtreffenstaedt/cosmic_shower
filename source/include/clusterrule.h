#ifndef CLUSTERRULE_H
#define CLUSTERRULE_H

#include <memory>
#include <string>

namespace Node {

class ParticleScorer;
class Cluster;
struct PrimaryParticle;

class ClusterRule {
public:
    explicit ClusterRule(std::shared_ptr<ParticleScorer> scorer);

    virtual ~ClusterRule() = default;
    enum class Result {
        NewCluster,
        SameCluster
    };

    auto update_cluster(std::shared_ptr<Cluster> cluster) -> void;

    [[nodiscard]] virtual auto result(const PrimaryParticle& particle) const -> Result;

protected:
    [[nodiscard]] auto current_cluster() const -> std::shared_ptr<Cluster>;

private:
    std::shared_ptr<ParticleScorer> m_scorer { nullptr };
    std::shared_ptr<Cluster> m_current_cluster { nullptr };
};

class SimpleClusterRule : public ClusterRule {
public:
    explicit SimpleClusterRule(std::shared_ptr<ParticleScorer> scorer);

    [[nodiscard]] auto result(const PrimaryParticle& particle) const -> Result override;

    ~SimpleClusterRule() override = default;

private:
    constexpr static std::size_t s_max { 100000 };
};

class FixedClusterRule : public ClusterRule {
public:
    explicit FixedClusterRule(std::shared_ptr<ParticleScorer> scorer, const std::string& secondaries);

    [[nodiscard]] auto result(const PrimaryParticle& particle) const -> Result override;

    ~FixedClusterRule() override = default;

private:
    constexpr static std::size_t s_number { 2 };
    constexpr static std::size_t s_default_max { 10000 };
    std::size_t m_max { s_default_max };
};

}

#endif // CLUSTERRULE_H
