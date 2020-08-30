#ifndef CORERUNNER_H
#define CORERUNNER_H

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "particlescorer.h"

namespace Node {

class Cluster;

class CoreRunner {
public:
    explicit CoreRunner(const std::string& directory, const std::string& configfile, const bool run = true);
    explicit CoreRunner(const std::string& directory, const std::string& configfile, const std::vector<std::string>& instances);
    virtual ~CoreRunner();

    auto register_instance(const std::string& name) -> void;

    [[nodiscard]] auto run() -> int;

    auto preserve(const bool state) -> void;

private:
    auto run(const std::string& name) -> void;

    std::string m_directory { "./primaries/" };

    std::queue<std::string> m_queued {};
    std::vector<std::future<void>> m_active {};

    std::atomic<bool> m_run { true };
    std::atomic<bool> m_locked { false };

    static constexpr std::size_t m_max_threads { 4 };

    std::mutex m_active_mutex {};
    std::mutex m_queued_mutex {};

    std::atomic<std::size_t> m_preserved;
};

}

#endif // CORERUNNER_H
