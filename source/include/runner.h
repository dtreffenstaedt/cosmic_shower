#ifndef RUNNER_H
#define RUNNER_H

#include "global.h"

#include <atomic>
#include <future>

namespace Node {

class ParticleDistributor;

class Runner
{
public:
    Runner(const std::string& name, const std::string& config_directory, const std::string& output_directory);

    auto run(ParticleDistributor& distributor) -> void;

    [[nodiscard]] auto done() const noexcept -> bool;

private:
    std::atomic<bool> m_done { false };
    std::future<void> m_future {};

    std::string m_name {};
    std::string m_config_directory {};
    std::string m_output_directory {};
};

}
#endif // RUNNER_H
