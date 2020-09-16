#include "particledistributor.h"

#include "cluster.h"
#include "clusterrule.h"
#include "corerunner.h"
#include "particlescorer.h"

#include <filesystem>
#include <iostream>
#include <algorithm>

namespace Node {

ParticleDistributor::ParticleDistributor(CoreRunner* runner, const std::shared_ptr<ParticleScorer>& scorer, const std::string& directory, std::string config)
    : m_runner { runner }
    , m_directory { directory }
    , m_config_file { config }
    , m_scorer { scorer }
    , m_future { run() }
{
    m_cluster_rule = std::make_shared<FixedClusterRule>(m_scorer, m_primaries);
}

ParticleDistributor::~ParticleDistributor()
{
    m_run = false;
    m_future.wait();
}

auto ParticleDistributor::run() -> std::future<void> {
    return std::async(std::launch::async, [this]{
        std::mutex mutex {};
        while (m_run || !m_primaries.empty()) {
            std::unique_lock<std::mutex> lock { mutex };
            m_has_primaries.wait(lock);
            distribute();
        }
    });
}

auto ParticleDistributor::pre_conditions() -> void {
    std::random_shuffle(m_primaries.begin(), m_primaries.end());
}

auto ParticleDistributor::distribute() -> void
{
    std::scoped_lock<std::mutex> lock { m_primary_mutex };
    pre_conditions();
    std::shared_ptr<Cluster> current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
    m_cluster_rule->update_cluster(current_cluster);
    for (auto& primary: m_primaries) {
        if (m_cluster_rule->result(primary) == ClusterRule::Result::NewCluster) {
            m_runner->register_instance(current_cluster->save());
            current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
            m_cluster_rule->update_cluster(current_cluster);
        }
        current_cluster->add(primary);
    }
    m_primaries.clear();
    m_runner->register_instance(current_cluster->save());
}

auto ParticleDistributor::collect(const std::string& secondaries) -> void {

    std::scoped_lock<std::mutex> lock { m_primary_mutex };

    parse(secondaries);
    m_has_primaries.notify_all();
}

auto ParticleDistributor::parse(const std::string& secondaries) -> void {
    std::ifstream stream {secondaries, std::ifstream::in};
    if (!stream.is_open()) {
        std::cout << "Error opening file '" << secondaries << "'\n"
                  << std::flush;
    }
    std::vector<PrimaryParticle> primaries {};
    while ((stream.good()) && (stream.peek() != EOF)) {
        while (std::isspace(stream.peek()) != 0) {
            (void)stream.get();
        } // skip any whitespace
        if (!(stream.good()) || (stream.peek() == EOF)) {
            break;
        }
        PrimaryParticle primary;
        constexpr std::size_t len { 50 };
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.particle = std::stoi(field);
            primary.n_particles = 1;
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.name = field;
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.origin.x = std::stod(field);
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.origin.y = std::stod(field);
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.origin.z = std::stod(field);
        }
        primary.origin.absolute = true;
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.momentum.x = std::stod(field);
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.momentum.y = std::stod(field);
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.momentum.z = std::stod(field);
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.momentum.m = std::stod(field);
        }
        {
            char field[len] = "";
            stream.get(field, len, ',');
            stream.get();
            primary.time.global = std::stod(field);
        }
        if (m_scorer->score(primary) > m_scorer->limit()) {
            m_primaries.push_back(primary);
        }
    }
    stream.close();
}
}
