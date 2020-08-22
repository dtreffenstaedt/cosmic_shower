#include "runner.h"

#include <fstream>
#include <filesystem>

#include "node.h"

namespace Node {

    Runner::Runner(const std::string& name, const std::string& config_directory, const std::string& output_directory)
        : m_name { name}
        , m_config_directory { config_directory }
        , m_output_directory { output_directory}
    {
    }

    auto Runner::run(ParticleDistributor &distributor) -> void {
        m_future = std::async(std::launch::async, [this]() -> void {
                                  std::string configfile {m_config_directory + "/" + m_name};
            if (system(("./run -c " + configfile).c_str()) != 0) {
                std::ofstream log {"node.log", std::fstream::out | std::fstream::app};
                log<<"Error executing for"<<configfile<<'\n'<<std::flush;
                log.close();
            }

                                  std::string secondaryfile {m_config_directory + "/" + m_name + "/event_1/secondaries"};

            if (std::filesystem::exists(secondaryfile))
            {
                distributor.register_particles(name, secondaryfile);
            }
                                  m_done = true;
                              });
    }

    auto Runner::done() const noexcept -> bool {
        return m_done;
    }
}
