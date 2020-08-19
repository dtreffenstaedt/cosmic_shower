#include "node.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include <libconfig.h++>
#include <utility>

namespace Node {

auto ParticleScorer::score(const PrimaryParticle& particle) const -> double
{
    double score { 0 };
    score += particle.momentum.m * -0.1;

    return score;
}

Cluster::Cluster(std::shared_ptr<ParticleScorer> scorer, std::string directory, std::string config)
    : m_scorer { std::move(scorer) }
    , m_directory { std::move(directory) }
    , m_config_file { std::move(config) }
{
}

auto Cluster::score() const -> double
{
    double total { 0 };

    for (const auto& particle : m_primaries) {
        total += m_scorer->score(particle);
    }

    return total;
}

auto Cluster::add(PrimaryParticle primary) -> void
{
    m_primaries.push_back(primary);
}

auto Cluster::number() const -> std::size_t
{
    return m_primaries.size();
}

auto Cluster::save() const -> void
{
    libconfig::Config config;
    config.readFile(m_config_file.c_str());
    libconfig::Setting& root = config.getRoot();
    std::string name {};
    root.lookupValue("name", name);
    if (root.exists("parent")) {
        root.lookup("parent") = name;
    } else {
        root.add("parent", libconfig::Setting::TypeString) = name;
    }
    if (name.find('+') > name.size()) {
        name += "+0+";
    }
    std::string last = name.substr(name.find('+') + 1);
    name = name.substr(0, name.find('+'));
    name += '+' + std::to_string(std::stoi(last.substr(0, last.find('+'))) + 1) + '+' + generate_id();
    root.lookup("name") = name;
    root.remove("primary");
    libconfig::Setting& primary_setting = root.add("primary", libconfig::Setting::TypeList);
    for (const auto& prim : m_primaries) {
        libconfig::Setting& primary = primary_setting.add(libconfig::Setting::TypeGroup);
        libconfig::Setting& origin = primary.add("origin", libconfig::Setting::TypeGroup);
        libconfig::Setting& momentum = primary.add("momentum", libconfig::Setting::TypeGroup);
        origin.add("x", libconfig::Setting::TypeFloat) = prim.origin.x;
        origin.add("y", libconfig::Setting::TypeFloat) = prim.origin.y;
        origin.add("z", libconfig::Setting::TypeFloat) = prim.origin.z;
        origin.add("absolute", libconfig::Setting::TypeBoolean) = prim.origin.absolute;

        momentum.add("x", libconfig::Setting::TypeFloat) = prim.momentum.x;
        momentum.add("y", libconfig::Setting::TypeFloat) = prim.momentum.y;
        momentum.add("z", libconfig::Setting::TypeFloat) = prim.momentum.z;
        momentum.add("magnitude", libconfig::Setting::TypeFloat) = prim.momentum.m;

        primary.add("n_particles", libconfig::Setting::TypeInt) = prim.n_particles;
        primary.add("particle", libconfig::Setting::TypeInt) = prim.particle;
        primary.add("name", libconfig::Setting::TypeString) = prim.name;
    }
    std::string configfile { m_directory + name };
    std::string secondaryfile {};
    root.lookupValue("data_directory", secondaryfile);
    secondaryfile += name + "/event_1/secondaries";
    config.writeFile((configfile).c_str());

    if (fork() == 0) {
        if (system(("./node " + configfile).c_str()) != 0) {
            std::ofstream log { "node.log", std::fstream::out | std::fstream::app };
            log << "Error executing for" << configfile << '\n'
                << std::flush;
            log.close();
            exit(-1);
        }
        exit(0);
    }
    /*    std::thread([configfile, secondaryfile]{
        if (system(("./run -c " + configfile).c_str()) != 0) {
            std::ofstream log {"node.log", std::fstream::out | std::fstream::app};
            log<<"Error executing for"<<configfile<<'\n'<<std::flush;
            log.close();
        }

        if (std::filesystem::exists(secondaryfile))
        {
            Node::ParticleDistributor distributor{"./sims/", configfile, secondaryfile};
            distributor.distribute();
        }
    }).detach();*/
}

auto Cluster::generate_id() -> std::string
{
    std::uint64_t point = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<std::uint64_t> dist(1, std::numeric_limits<std::uint64_t>::max());

    std::stringstream stream {};
    stream << std::hex << (point ^ dist(e2));
    return stream.str();
}

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
            current_cluster->save();
            current_cluster = std::make_shared<Cluster>(m_scorer, m_directory, m_config_file);
            m_clusters.push_back(current_cluster);
            m_cluster_rule->update_cluster(current_cluster);
        }
        current_cluster->add(primary);
    }
    current_cluster->save();
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

auto main(const int argc, const char* argv[]) -> int
{
    if (argc != 2) {
        std::cout << "Need parameter to config file.\n";
        return -1;
    }
    std::string configfile { argv[1] };
    std::string secondaryfile {};
    libconfig::Config config;
    config.readFile(configfile.c_str());
    libconfig::Setting& root = config.getRoot();
    std::string name {};
    root.lookupValue("name", name);

    root.lookupValue("data_directory", secondaryfile);
    secondaryfile += "/" + name + "/event_1/secondaries";
    if (system(("./run -c " + configfile).c_str()) != 0) {
        std::cerr << "Error running simulation!\n";
    }

    if (std::filesystem::exists(secondaryfile)) {
        std::cout << "Starting sub simulations\n";
        Node::ParticleDistributor distributor { "./sims/", configfile, secondaryfile };
        distributor.distribute();
    } else {
        std::cout << " simulation is done\n";
    }
    return 0;
}
