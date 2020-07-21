#include "recordermanager.h"

#include "configmanager.h"
#include "parametermanager.h"

#include <filesystem>
#include <fstream>

namespace Shower {
RecorderManager* RecorderManager::c_singleton { nullptr };

RecorderManager::RecorderManager()
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

    double world_size { ConfigManager::singleton()->get_world_size() * m };
    for (size_t i { 0 }; i < m_size; i++) {
        for (size_t j { 0 }; j < m_size; j++) {
            m_bins[i][j].initialise(i, j, world_size);
        }
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
    file << "x[mm],y[mm],z[mm],time[ns]\n";

    file << m_primary.pos.x() << ',' << m_primary.pos.y() << ',' << m_primary.pos.z() << ',' << m_primary.time << "# primary\n";
    while (!m_hits.empty()) {
        Hit h = m_hits.front();
        m_hits.pop();
        file << h.pos.x() << ',' << h.pos.y() << ',' << h.pos.z() << ',' << h.time << "\n";
    }

    file.close();

    std::ofstream bin_file(dir + "/momentum_density", std::ofstream::app);
    if (!bin_file.is_open()) {
        return;
    }
    bin_file << "x[mm],y[mm],density\n";

    for (size_t i { 0 }; i < m_size; i++) {
        for (size_t j { 0 }; j < m_size; j++) {
            bin_file << m_bins[i][j].get_x_center() << ',' << m_bins[i][j].get_y_center() << ',' << m_bins[i][j].get_value() << '\n';
        }
    }

    bin_file.close();
}

void RecorderManager::store_momentum(const G4ThreeVector& vector, const G4double& momentum)
{
    for (size_t i { 0 }; i < m_size; i++) {
        for (size_t j { 0 }; j < m_size; j++) {
            if (m_bins[i][j].in_bin(vector.x(), vector.y())) {
                m_bins[i][j].store_momentum(momentum);
                return;
            }
        }
    }
}

template <size_t N>
void RecorderManager::Bin<N>::initialise(const size_t& x, const size_t& y, const double& world_size)
{
    m_x_min = world_size * (static_cast<double>(x) / static_cast<double>(N) - 0.5);
    m_x_max = world_size * (static_cast<double>(x + 1) / static_cast<double>(N) - 0.5);
    m_y_min = world_size * (static_cast<double>(y) / static_cast<double>(N) - 0.5);
    m_y_max = world_size * (static_cast<double>(y + 1) / static_cast<double>(N) - 0.5);
    m_inverse_bin_area = std::pow((1.0e6 * static_cast<double>(N)) / (world_size), 2.0); // area is in km2
}

template <size_t N>
auto RecorderManager::Bin<N>::in_bin(const double& x, const double& y) const -> bool
{
    return (x > m_x_min) and (x < m_x_max) and (y > m_y_min) and (y < m_y_max);
}
template <size_t N>
void RecorderManager::Bin<N>::store_momentum(const double& momentum)
{
    m_value += momentum * m_inverse_bin_area;
}

template <size_t N>
auto RecorderManager::Bin<N>::get_value() const -> double
{
    return m_value;
}

template <size_t N>
auto RecorderManager::Bin<N>::get_x_center() const -> double
{
    return (m_x_max + m_x_min) * 0.5;
}

template <size_t N>
auto RecorderManager::Bin<N>::get_y_center() const -> double
{
    return (m_y_max + m_y_min) * 0.5;
}
}
