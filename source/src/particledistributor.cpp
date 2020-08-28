#include "particledistributor.h"

#include "cluster.h"
#include "clusterrule.h"
#include "corerunner.h"
#include "particlescorer.h"

#include <filesystem>
#include <iostream>
#include <algorithm>

namespace Node {

ParticleDistributor::ParticleDistributor(CoreRunner* runner, const std::string& directory, std::string config, const std::string& secondaries)
    : m_directory { directory }
    , m_config_file { std::move(config) }
    , m_secondaries { secondaries }
    , m_stream { secondaries, std::ifstream::in }
    , m_runner { runner }
{
    if (!m_stream.is_open()) {
        std::cout << "Error opening file '" << secondaries << "'\n"
                  << std::flush;
    }
    runner->preserve(true);
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directory(directory);
    }
    parse();
    m_cluster_rule = std::make_shared<FixedClusterRule>(m_scorer, m_primaries);
}

ParticleDistributor::~ParticleDistributor()
{
    m_primaries.clear();
    m_stream.close();
    m_runner->preserve(false);
}

auto ParticleDistributor::pre_conditions() -> void {
    std::random_shuffle(m_primaries.begin(), m_primaries.end());
}

auto ParticleDistributor::distribute() -> void
{
    pre_conditions();
    m_clusters.clear();
    std::shared_ptr<Cluster> current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
    m_clusters.push_back(current_cluster);
    m_cluster_rule->update_cluster(current_cluster);
    for (auto& primary: m_primaries) {
        if (m_cluster_rule->result(primary) == ClusterRule::Result::NewCluster) {
            m_runner->register_instance(current_cluster->save());
            current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
            m_clusters.push_back(current_cluster);
            m_cluster_rule->update_cluster(current_cluster);
        }
        current_cluster->add(primary);
    }
    m_primaries.clear();
    m_runner->register_instance(current_cluster->save());
}

auto ParticleDistributor::parse() -> void
{
    while ((m_stream.good()) && (m_stream.peek() != EOF)) {
        while (std::isspace(m_stream.peek()) != 0) {
            (void)m_stream.get();
        } // skip any whitespace
        if (!(m_stream.good()) || (m_stream.peek() == EOF)) {
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
        if (m_scorer->score(primary) > m_scorer->limit()) {
            m_primaries.push_back(primary);
        }
    }
}
}
