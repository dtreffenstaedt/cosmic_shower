#ifndef NODE_H
#define NODE_H

#include <fstream>
#include <memory>
#include <queue>
#include <vector>

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

class Cluster {
public:
    explicit Cluster(std::shared_ptr<ParticleScorer> scorer, const std::string& directory, const std::string& config);

    [[nodiscard]] auto score() const -> double;

    auto add(PrimaryParticle primary) -> void;

    [[nodiscard]] auto number() const -> std::size_t;

    auto save() const -> void;

private:
    [[nodiscard]] auto generate_id() const -> std::string;
    std::shared_ptr<ParticleScorer> m_scorer { nullptr };
    std::vector<PrimaryParticle> m_primaries {};

    std::string m_directory { "./primaries/" };
    std::string m_config_file { "shower.cfg" };
};

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

class ParticleDistributor {
public:
    explicit ParticleDistributor(const std::string& directory, const std::string& config, const std::string& secondaries);
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
};

}
#endif // NODE_H
