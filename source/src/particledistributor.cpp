#include "particledistributor.h"

#include "cluster.h"
#include "clusterrule.h"
#include "particlescorer.h"

#include <filesystem>

namespace Node {

ParticleDistributor::ParticleDistributor(const std::string& directory, std::string config, const std::string& secondaries)
    : m_directory { directory }
    , m_config_file { std::move(config) }
    , m_secondaries { secondaries }
    , m_cluster_rule { std::make_shared<FixedClusterRule>(m_scorer, secondaries) }
    , m_stream { m_secondaries, std::ifstream::in }
{
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directory(directory);
    }
    parse();
}

ParticleDistributor::~ParticleDistributor()
{
    m_stream.close();
    for (auto& thread : m_threads) {
        thread.get();
    }
}

auto ParticleDistributor::distribute() -> void
{
    m_clusters.clear();
    std::shared_ptr<Cluster> current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
    m_clusters.push_back(current_cluster);
    m_cluster_rule->update_cluster(current_cluster);
    while (has_next()) {
        PrimaryParticle primary { next() };

        if (m_cluster_rule->result(primary) == ClusterRule::Result::NewCluster) {
            m_threads.push_back(current_cluster->save());
            current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
            m_clusters.push_back(current_cluster);
            m_cluster_rule->update_cluster(current_cluster);
        }
        current_cluster->add(primary);
    }
    m_threads.push_back(current_cluster->save());
}

auto ParticleDistributor::parse() -> void
{
    if (m_end || (m_primaries.size() > 10)) {
        return;
    }
    for (std::size_t i { 0 }; i < 100; i++) {
        while (std::isspace(m_stream.peek()) != 0) {
            (void)m_stream.get();
        } // skip any whitespace
        if (!(m_stream.good()) || (m_stream.peek() == EOF)) {
            m_end = true;
            return;
        }
        PrimaryParticle primary;
        constexpr std::size_t len { 50 };
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.particle = std::stoi(field);
            primary.n_particles = 1;
        }
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.name = field;
        }
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.origin.x = std::stod(field);
        }
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.origin.y = std::stod(field);
        }
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.origin.z = std::stod(field);
        }
        primary.origin.absolute = true;
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.momentum.x = std::stod(field);
        }
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.momentum.y = std::stod(field);
        }
        {
            char field[len] = "";
            m_stream.get(field, len, ',');
            m_stream.get();
            primary.momentum.z = std::stod(field);
        }
        {
            char field[len] = "";
            m_stream.get(field, len, '\n');
            m_stream.get();
            primary.momentum.m = std::stod(field);
        }
        m_primaries.push(primary);
    }
}

auto ParticleDistributor::has_next() const -> bool
{
    return !(m_primaries.empty());
}

auto ParticleDistributor::next() -> PrimaryParticle
{
    parse();
    PrimaryParticle primary { m_primaries.front() };
    m_primaries.pop();
    return primary;
}
}
