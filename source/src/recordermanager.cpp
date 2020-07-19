#include "recordermanager.h"

#include "configmanager.h"
#include "parametermanager.h"

#include <filesystem>
#include <fstream>

namespace Shower {
RecorderManager* RecorderManager::c_singleton = nullptr;

RecorderManager::RecorderManager()
    : m_stored_primary { false }

{
    std::string data = ConfigManager::singleton()->get_data_directory();
    std::string dir = data + "/" + ConfigManager::singleton()->get_name();

    if (!ParameterManager::singleton()->argument_set("o")) {
        if (std::filesystem::exists(dir + "record")) {
            throw DataDirectoryExists();
        }
    }

    if (!std::filesystem::exists(data)) {
        std::filesystem::create_directory(data);
    }

    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directory(dir);
    }
    c_singleton = this;
}

RecorderManager::~RecorderManager()
{
    save();
}

auto RecorderManager::singleton() -> RecorderManager*
{
    return c_singleton;
}

auto RecorderManager::stored_primary() const -> bool
{
    return m_stored_primary;
}

void RecorderManager::store_primary(const G4ThreeVector& vector, const G4double& time)
{
    m_primary.pos = vector;
    m_primary.time = time;
    m_stored_primary = true;
}

void RecorderManager::store_hit(const G4ThreeVector& vector, const G4double& time)
{
    m_hits.push({ vector, time });
}

void RecorderManager::save()
{
    std::string dir = ConfigManager::singleton()->get_data_directory() + "/" + ConfigManager::singleton()->get_name();

    std::ofstream file(dir + "/record", std::ofstream::app);
    if (!file.is_open()) {
        return;
    }
    file << "x,y,z,time\n";

    file << m_primary.pos.x() << ',' << m_primary.pos.y() << ',' << m_primary.pos.z() << ',' << m_primary.time << "# primary\n";
    while (!m_hits.empty()) {
        Hit h = m_hits.front();
        m_hits.pop();
        file << h.pos.x() << ',' << h.pos.y() << ',' << h.pos.z() << ',' << h.time << "\n";
    }

    file.close();
}
}
