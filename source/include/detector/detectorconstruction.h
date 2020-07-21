#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>

#include <globals.hh>

#include <variant>

#include "configmanager.h"
#include "global.h"
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace Shower {
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();

    ~DetectorConstruction() override;

    auto Construct() -> G4VPhysicalVolume* override;
    void ConstructSDandField() override;

protected:
    std::variant<std::vector<Config::DetectorPlacement>, size_t> m_detectors { ConfigManager::singleton()->get_detectors() };
    std::vector<Config::AtmosphereLayer> m_atmosphere_layers { ConfigManager::singleton()->get_atmosphere_layers() };
    G4double m_world_size { ConfigManager::singleton()->get_world_size() * m };
    G4double m_atmosphere_height { ConfigManager::singleton()->get_atmosphere_height() * m };
    Config::MagneticField m_magnetic_field { ConfigManager::singleton()->get_magnetic_field() };
    Config::DetectorProperties m_detector_properties { ConfigManager::singleton()->get_detector_properties() };
    Config::PrimaryParticle m_primary_particle { ConfigManager::singleton()->get_primary_particle() };
    G4double m_theta { std::atan(std::sqrt(std::pow(m_primary_particle.momentum.x, 2) + std::pow(m_primary_particle.momentum.y, 2)) / m_primary_particle.momentum.z) * radian };
    G4double m_phi { std::atan2(m_primary_particle.momentum.y, m_primary_particle.momentum.x) * radian };

    struct
    {
        G4double x;
        G4double y;
        G4double z;
    } m_offset_bottom {
        m_atmosphere_height * m_primary_particle.momentum.x / (-m_primary_particle.momentum.z) / 2,
        m_atmosphere_height* m_primary_particle.momentum.y / (-m_primary_particle.momentum.z) / 2,
        -m_atmosphere_height / 2
    };

    G4LogicalVolume* m_world_logical { nullptr };
    G4LogicalVolume* m_air_logical {};

    std::vector<G4LogicalVolume*> m_logical_volumes;
    std::vector<G4VPhysicalVolume*> m_physical_layers;
    std::vector<G4VSolid*> m_solid_layers;

    G4VSolid* m_detector_geometry { nullptr };
    G4Material* m_detector_material { nullptr };
    std::vector<G4LogicalVolume*> m_detector_logicals {};
    G4VPhysicalVolume* m_intensity_catcher_physical { nullptr };

    auto construct_world() -> G4VPhysicalVolume*;
    void construct_atmosphere();
    void construct_magnetic_field();
    void construct_detectors();
    void define_detectors();

    void place_detector(const std::string& name, const G4double& x, const G4double& y, const G4double& z);
};
}

#endif // DETECTORCONSTRUCTION_H
