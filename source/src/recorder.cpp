#include "recorder.h"

#include <filesystem>
#include <fstream>

namespace Shower {
Recorder::Recorder(const std::shared_ptr<Configuration>& config, const std::shared_ptr<Parameters>& params)
{
    std::string data = config->get_data_directory();
    m_directory = data + "/" + config->get_name();

    if (!params->argument_set("o")) {
        if (std::filesystem::exists(m_directory)) {
            throw DataDirectoryExists();
        }
    } else {
        std::filesystem::remove(m_directory);
    }

    if (!std::filesystem::exists(data)) {
        std::filesystem::create_directory(data);
    }

    if (!std::filesystem::exists(m_directory)) {
        std::filesystem::create_directory(m_directory);
    }

    double world_size { config->get_world_size()};
    for (size_t i { 0 }; i < m_size; i++) {
        for (size_t j { 0 }; j < m_size; j++) {
            m_bins[i][j].initialise(i, j, world_size);
        }
    }

    config->config_dump(m_directory + "/config_dunp");

}

Recorder::~Recorder()
{
    save();
}

auto Recorder::stored_primary() const -> bool
{
    return m_stored_primary;
}

void Recorder::store_primary(const Hit& primary)
{
    m_primary.position = primary.position;
    m_primary.time = primary.time;
    m_stored_primary = true;
}

void Recorder::store_hit(const Hit& hit)
{
    m_hits.push(hit);
}

void Recorder::store_detailed_hit(const DetailedHit& hit)
{
    m_detailed_hits.push(hit);
    if (std::abs(hit.pdg) == 13) {
        store_hit({ hit.position, hit.global_time });
    }
}

void Recorder::save()
{
    {
        std::ofstream file(m_directory + "/hits", std::ofstream::app);
        if (!file.is_open()) {
            return;
        }
        file << "x[m],y[m],z[m],time[ns]\n";

        file << m_primary.position.x() / meter << ',' << m_primary.position.y() / meter << ',' << m_primary.position.z() / meter << ',' << m_primary.time << "# primary\n";
        while (!m_hits.empty()) {
            Hit h = m_hits.front();
            m_hits.pop();
            file << h.position.x() << ',' << h.position.y() << ',' << h.position.z() << ',' << h.time << "\n";
        }

        file.close();
    }
    {
        std::ofstream file(m_directory + "/detailed_hits", std::ofstream::app);
        if (!file.is_open()) {
            return;
        }
        file << "Particle,Detector,position_x[m],position_y[m],position_z[m],momentum_x,momentum_y,momentum_z,momentum_magnitude[MeV],global_time[ns],proper_time[ns]\n";

        while (!m_detailed_hits.empty()) {
            DetailedHit h = m_detailed_hits.front();
            m_detailed_hits.pop();
            file
                << h.pdg << ','
                << h.position.x() / meter << ',' << h.position.y() / meter << ',' << h.position.z() / meter << ','
                << h.momentum_direction.x() << ',' << h.momentum_direction.y() << ',' << h.momentum_direction.z() << ','
                << h.momentum / MeV << ','
                << h.global_time / ns << ',' << h.proper_time / ns << '\n';
        }

        file.close();
    }
    {
        std::ofstream file(m_directory + "/ground_intensity", std::ofstream::app);
        if (!file.is_open()) {
            return;
        }
        file << "x[m],y[m],rest energy[MeV],kinetic energy[MeV],n charged,n uncharged\n";

        for (size_t i { 0 }; i < m_size; i++) {
            for (size_t j { 0 }; j < m_size; j++) {
                m_bins[i][j].store(file);
            }
        }

        file.close();
    }
}

void Recorder::store_ground_intensity(const GroundIntensity& intensity)
{
    for (size_t i { 0 }; i < m_size; i++) {
        for (size_t j { 0 }; j < m_size; j++) {
            if (m_bins[i][j].in_bin(intensity.position.x(), intensity.position.y())) {
                m_bins[i][j].store_momentum(intensity.rest_energy);
                m_bins[i][j].store_energy(intensity.energy);
                if (intensity.charged) {
                    m_bins[i][j].store_charged();
                } else {
                    m_bins[i][j].store_uncharged();
                }
                return;
            }
        }
    }
}

template <size_t N>
void Recorder::Bin<N>::initialise(const size_t& x, const size_t& y, const double& world_size)
{
    m_x_min = world_size * (static_cast<double>(x) / static_cast<double>(N) - 0.5);
    m_x_max = world_size * (static_cast<double>(x + 1) / static_cast<double>(N) - 0.5);
    m_y_min = world_size * (static_cast<double>(y) / static_cast<double>(N) - 0.5);
    m_y_max = world_size * (static_cast<double>(y + 1) / static_cast<double>(N) - 0.5);
    m_inverse_bin_area = std::pow((1.0e6 * static_cast<double>(N)) / (world_size), 2.0); // area is in km2
}

template <size_t N>
auto Recorder::Bin<N>::in_bin(const double& x, const double& y) const -> bool
{
    return (x > m_x_min) and (x < m_x_max) and (y > m_y_min) and (y < m_y_max);
}
template <size_t N>
void Recorder::Bin<N>::store_momentum(const double& momentum)
{
    m_momentum_density += momentum; // * m_inverse_bin_area;
}
template <size_t N>
void Recorder::Bin<N>::store_energy(const double& energy)
{
    m_energy_density += energy; // * m_inverse_bin_area;
}
template <size_t N>
void Recorder::Bin<N>::store_charged()
{
    m_n_charged++;
}
template <size_t N>
void Recorder::Bin<N>::store_uncharged()
{
    m_n_uncharged++;
}

template <size_t N>
auto Recorder::Bin<N>::get_x_center() const -> double
{
    return (m_x_max + m_x_min) * 0.5;
}

template <size_t N>
auto Recorder::Bin<N>::get_y_center() const -> double
{
    return (m_y_max + m_y_min) * 0.5;
}

template <size_t N>
void Recorder::Bin<N>::store(std::ofstream& stream)
{
    stream << get_x_center() / meter << ',' << get_y_center() / meter << ',' << m_momentum_density << ',' << m_energy_density << ',' << std::to_string(m_n_charged) << ',' << std::to_string(m_n_uncharged) << '\n';
}
}