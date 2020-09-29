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
    m_cluster_rule = std::make_shared<ScoreClusterRule>(m_scorer);
}

ParticleDistributor::~ParticleDistributor()
{
    m_run = false;
    m_future.wait();
}

auto ParticleDistributor::empty() const -> bool {
    return (m_primaries.empty() && (m_current_cluster == nullptr));
}
auto ParticleDistributor::size() const -> size_t {
    return m_primaries.size();
}

auto ParticleDistributor::run() -> std::future<void> {
    return std::async(std::launch::async, [this]{
        std::mutex mutex {};
        while (m_run || !m_primaries.empty()) {
            std::unique_lock<std::mutex> lock { mutex };
            if (m_has_primaries.wait_for(lock, std::chrono::seconds{1}) == std::cv_status::no_timeout) {
                distribute();
            }
        }
    });
}

auto ParticleDistributor::pre_conditions() -> void {
    std::random_shuffle(m_primaries.begin(), m_primaries.end());
}

auto ParticleDistributor::distribute(const bool force) -> void
{
    std::cout << "Distributing\n"<< std::flush;
    std::scoped_lock<std::mutex> lock { m_primary_mutex };
    pre_conditions();
    if (m_current_cluster == nullptr) {
        m_current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
        m_cluster_rule->update_cluster(m_current_cluster);
    }

    for (auto& primary: m_primaries) {
        if (m_cluster_rule->result(primary) == ClusterRule::Result::NewCluster) {
            m_runner->register_instance(m_current_cluster->save());
            m_current_cluster.reset();
            m_current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
            m_cluster_rule->update_cluster(m_current_cluster);
        }
        m_current_cluster->add(primary);
    }
    m_primaries.clear();
    if (m_current_cluster->number() == 0) {
        m_current_cluster.reset();
        m_current_cluster = nullptr;
        m_cluster_rule->update_cluster(m_current_cluster);
    } else if (force) {
        m_runner->register_instance(m_current_cluster->save());
        m_current_cluster.reset();
        m_current_cluster = nullptr;
        m_cluster_rule->update_cluster(m_current_cluster);
    }
}

auto ParticleDistributor::collect(const std::string& secondaries) -> void {
    parse(secondaries);
    m_has_primaries.notify_all();
}

auto ParticleDistributor::parse(const std::string& secondaries) -> void {
    std::ifstream stream {secondaries, std::ifstream::in};
    if (!stream.is_open()) {
        std::cout << "Error opening file '" << secondaries << "'\n"
                  << std::flush;
    }
    std::scoped_lock<std::mutex> lock { m_primary_mutex };
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
            stream.get(field, len, '\n');
            stream.get();
            primary.time.global = std::stod(field);
        }
        if (m_scorer->score(primary) > m_scorer->limit()) {
            m_queued_energy += m_scorer->score(primary);
            m_primaries.push_back(primary);
        }
    }
    stream.close();
}
}
