#ifndef CLUSTER_H
#define CLUSTER_H

#include <future>
#include <memory>
#include <string>
#include <vector>

namespace Node {

class ParticleScorer;
struct PrimaryParticle;

class Cluster {
public:
    explicit Cluster(std::shared_ptr<ParticleScorer> scorer, std::string directory, std::string config);

    [[nodiscard]] auto score() const -> double;

    auto add(PrimaryParticle primary) -> void;

    [[nodiscard]] auto number() const -> std::size_t;

    [[nodiscard]] auto save() const -> std::string;

private:
    [[nodiscard]] static auto generate_id() -> std::string;
    std::shared_ptr<ParticleScorer> m_scorer { nullptr };
    std::vector<PrimaryParticle> m_primaries {};

    std::string m_directory { "./primaries/" };
    std::string m_config_file { "shower.cfg" };
};

}

#endif // CLUSTER_H
