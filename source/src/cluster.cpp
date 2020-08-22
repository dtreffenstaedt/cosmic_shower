#include "cluster.h"

#include "particledistributor.h"
#include "particlescorer.h"

#include <libconfig.h++>

#include <filesystem>
#include <fstream>
#include <random>

namespace Node {

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

auto Cluster::save() const -> std::future<void>
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
    /*
    if (fork() == 0) {
        if (system(("./node " + configfile).c_str()) != 0) {
            std::ofstream log { "node.log", std::fstream::out | std::fstream::app };
            log << "Error executing for" << configfile << '\n'
                << std::flush;
            log.close();
            exit(-1);
        }
        exit(0);
    }*/
    return std::async(std::launch::async, [configfile, secondaryfile] {
        if (system(("./run -c " + configfile).c_str()) != 0) {
            std::ofstream log { "node.log", std::fstream::out | std::fstream::app };
            log << "Error executing for" << configfile << '\n'
                << std::flush;
            log.close();
        }

        if (std::filesystem::exists(secondaryfile)) {
            Node::ParticleDistributor distributor { "./sims/", configfile, secondaryfile };
            distributor.distribute();
        }
    });
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
}
