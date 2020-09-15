#include "particlecollector.h"

#include <filesystem>
#include <iostream>

namespace Node {

ParticleCollector::ParticleCollector(const std::string& directory, const std::string& name)
    : m_directory { directory }
    , m_name { name }
    , m_detailed_hits { "./data/" + name + ".full/detailed_hits", std::fstream::out | std::fstream::app }
    , m_hits { "./data/" + name + ".full/hits", std::fstream::out | std::fstream::app }
    , m_ground_intensity { "./data/" + name + ".full/ground_intensity", std::fstream::out | std::fstream::app }
{
    m_detailed_hits<< "Particle,Detector,position_x[m],position_y[m],position_z[m],energy_x,energy_y,energy_z,energy_magnitude[MeV],global_time[ns],proper_time[ns]\n";
    m_hits << "x[m],y[m],z[m],time[ns]\n";
    m_ground_intensity << "x[m],y[m],rest energy[MeV],kinetic energy[MeV],n charged,n uncharged\n";
}

auto ParticleCollector::collect(const std::string& name) -> void {
    const std::string dir {m_directory + "/" + name + "/event_1/"};
    collect_intensity(dir + "ground_intensity");
    collect(dir + "detailed_hits", m_detailed_hits);
    collect(dir + "hits", m_hits);
}

auto ParticleCollector::collect(const std::string& name, std::ofstream& stream) -> void {
    if (!std::filesystem::exists(name)) {
        return;
    }
    std::ifstream file {name, std::fstream::in};

    if (!file.is_open()) {
        std::cerr<<"Failed to open file '"<<name<<"'\n"<<std::flush;
        return;
    }

    std::string line {};
    std::getline(file, line, '\n'); // first line is the header
    while (true) {
        line.clear();
        std::getline(file, line, '\n');
        if (!file.good() || (file.peek() == EOF) || (std::empty(line))) {
            break;
        }
        stream<<line;
    }
    file.close();
}

auto ParticleCollector::collect_intensity(const std::string& dir) -> void {
    if (!std::filesystem::exists(dir)) {
        return;
    }
    std::ifstream file {dir, std::fstream::in};

    if (!file.is_open()) {
        std::cerr<<"Failed to open file '"<<dir<<"'\n"<<std::flush;
        return;
    }

    file.close();
}

auto ParticleCollector::save() -> void {

}
}
