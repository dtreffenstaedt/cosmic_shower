#include "corerunner.h"

#include "cluster.h"
#include "particledistributor.h"

#include <algorithm>
#include <filesystem>
#include <iostream>

#include <libconfig.h++>

namespace Node {

CoreRunner::CoreRunner(const std::string& directory)
    : m_directory { directory }
{
}

CoreRunner::~CoreRunner()
{
    m_run = false;
}

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
    while (m_run) {
        if (m_active.empty() && m_queued.empty() && (m_preserved == 0)) {
            std::cout << "Quitting\n"
                      << std::flush;
            m_run = false;
            break;
        }
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

auto CoreRunner::preserve(const bool state) -> void
{
    if (state) {
        m_preserved++;
    } else {
        m_preserved--;
    }
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
        std::string configfile { m_directory + "/" + name };
        if (system(("./run -c " + configfile).c_str()) != 0) {
            std::ofstream log { "node.log", std::fstream::out | std::fstream::app };
            log << "Error executing for" << configfile << '\n'
                << std::flush;
            log.close();
        }

        std::string secondaryfile { output_directory + "event_1/secondaries" };

        std::cout << "Checking for secondaries: " << secondaryfile << '\n'
                  << std::flush;
        if (std::filesystem::exists(secondaryfile)) {
            ParticleDistributor distributor { this, m_directory, name, secondaryfile };
            distributor.distribute();
        } else {
            std::cout << "Not found.\n"
                      << std::flush;
        }
    }));
}

}
