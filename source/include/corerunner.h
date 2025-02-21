#ifndef CORERUNNER_H
#define CORERUNNER_H

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

#include "particlescorer.h"

namespace Node {

class Cluster;
class ParticleDistributor;
class ParticleCollector;

class CoreRunner {
public:
    explicit CoreRunner(const std::string& directory, const std::string& configfile, const bool run = true);
    explicit CoreRunner(const std::string& directory, const std::string& configfile, const std::vector<std::string>& instances);
    virtual ~CoreRunner();

    auto register_instance(const std::string& name) -> void;

    [[nodiscard]] auto run() -> int;


private:
    auto run(const std::string& name) -> void;

    void write_data_point();

    std::string m_directory { "./primaries/" };
    std::ofstream m_data_file { "node.dat" };

    std::queue<std::string> m_queued {};
    std::vector<std::future<int>> m_active {};

    std::atomic<bool> m_locked { false };

    std::atomic<bool> m_run { true };

    static constexpr std::size_t m_max_threads { 6 };

    std::mutex m_active_mutex {};
    std::mutex m_queued_mutex {};

    std::atomic<std::size_t> m_running { 0 };
    std::atomic<std::size_t> m_total { 0 };
    std::atomic<std::size_t> m_n_queued { 0 };
    std::atomic<std::size_t> m_finished { 0 };
    std::atomic<std::size_t> m_failed { 0 };
    double m_absorbed { 0.0 };

    std::chrono::system_clock::time_point m_start { std::chrono::system_clock::now() };
    std::chrono::system_clock::time_point m_last { std::chrono::system_clock::now() };

    std::unique_ptr<ParticleDistributor> m_distributor { nullptr };
    std::unique_ptr<ParticleCollector> m_collector { nullptr };
};

}

#endif // CORERUNNER_H
