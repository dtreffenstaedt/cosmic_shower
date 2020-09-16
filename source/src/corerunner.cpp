#include "corerunner.h"

#include "cluster.h"
#include "particledistributor.h"
#include "particlecollector.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <ctime>

#include <libconfig.h++>

namespace Node {

CoreRunner::CoreRunner(const std::string& directory, const std::string& configfile, const bool run)
    : m_directory { directory }
    , m_distributor { std::make_unique<ParticleDistributor> (this, std::make_shared<ParticleScorer>(), directory, configfile)}
{
    libconfig::Config config;
    config.readFile((m_directory + "/" + configfile).c_str());
    libconfig::Setting& root = config.getRoot();

    std::string output_directory;
    root.lookupValue("data_directory", output_directory);
    m_collector = std::make_unique<ParticleCollector> (output_directory, configfile);

    if (run) {
        register_instance(configfile);
    }
}

CoreRunner::CoreRunner(const std::string& directory, const std::string& configfile, const std::vector<std::string>& instances)
    : CoreRunner{directory, configfile, false}
{
    for (const auto& instance: instances) {
        register_instance(instance);
    }
}

CoreRunner::~CoreRunner() = default;

auto CoreRunner::register_instance(const std::string& name) -> void
{
    std::scoped_lock<std::mutex, std::mutex> lock { m_active_mutex, m_queued_mutex };
    m_locked = true;
    if (m_active.size() < m_max_threads) {
        run(name);
    } else {
        m_queued.push(name);
        std::cout << "Holding Event '" << name << "' (" << m_queued.size() << " in queue)\n"
                  << std::flush;
    }
    m_locked = false;
}

auto CoreRunner::run() -> int
{
    while (!m_active.empty() || !m_queued.empty() || (m_running > 0)) {
        if (!m_locked) {
            m_locked = true;
            std::scoped_lock<std::mutex> lock { m_active_mutex };
            std::vector<std::size_t> remove {};
            for (std::size_t i { 0 }; i < m_active.size(); i++) {
                if (m_active[i].wait_for(std::chrono::microseconds(10)) == std::future_status::ready) {
                    remove.push_back(i);
                }
            }
            std::sort(remove.rbegin(), remove.rend());
            for (const auto& i : remove) {
                m_active.erase(m_active.begin() + static_cast<long>(i));
            }
            m_locked = false;
        }

        if (!m_locked) {
            m_locked = true;
            std::scoped_lock<std::mutex, std::mutex> lock { m_active_mutex, m_queued_mutex };
            if (m_active.size() < m_max_threads) {
                if (!m_queued.empty()) {
                    run(m_queued.front());
                    std::cout << "Getting Event '" << m_queued.front() << "' (" << m_queued.size() - 1 << " in queue)\n"
                              << std::flush;
                    m_queued.pop();
                }
            }
            m_locked = false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

auto CoreRunner::run(const std::string& name) -> void
{
    libconfig::Config config;
    config.readFile((m_directory + "/" + name).c_str());
    libconfig::Setting& root = config.getRoot();

    std::string output_directory;
    root.lookupValue("data_directory", output_directory);
    output_directory += "/" + name + "/";

    m_active.push_back(std::async(std::launch::async, [this, name, output_directory]() -> void {
        const auto start = std::chrono::system_clock::now();
        m_running++;
        std::string configfile { m_directory + "/" + name };
        if (system(("./run -c " + configfile).c_str()) != 0) {
            std::ofstream log { "node.log", std::fstream::out | std::fstream::app };
            log << "Error executing for" << configfile << '\n'
                << std::flush;
            log.close();
        }
        const auto end = std::chrono::system_clock::now();

        std::string secondaryfile { output_directory + "event_1/secondaries" };
        std::string runinfofile { output_directory + "runinfo" };

        std::ofstream out{runinfofile};
        std::time_t start_t = std::chrono::system_clock::to_time_t(start);
        std::time_t end_t = std::chrono::system_clock::to_time_t(end);

        out<<"start: "<<std::put_time(std::localtime(&start_t), "%F %T")
          <<"\nend: "<<std::put_time(std::localtime(&end_t), "%F %T")
         <<"\nduratiton: "<<std::chrono::duration_cast<std::chrono::minutes>(end - start).count()<<'\n';



        std::cout << "Checking for secondaries: " << secondaryfile << '\n'
                  << std::flush;
        if (std::filesystem::exists(secondaryfile)) {
            m_distributor->collect(secondaryfile);
        } else {
            std::cout << "Not found.\n"
                      << std::flush;
        }
        m_collector->collect(name);
        m_running--;
    }));
}

}
