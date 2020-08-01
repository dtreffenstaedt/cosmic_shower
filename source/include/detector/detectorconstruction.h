#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>

#include <globals.hh>

#include <variant>

#include "configuration.h"
#include "global.h"
#include "recorder.h"
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace Shower {
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction(std::shared_ptr<Recorder> recorder, const std::shared_ptr<Configuration>& configuration);

    ~DetectorConstruction() override;

    auto Construct() -> G4VPhysicalVolume* override;
    void ConstructSDandField() override;

protected:
    std::shared_ptr<Recorder> m_recorder { nullptr };
    std::shared_ptr<Configuration> m_configuration { nullptr };
    std::variant<std::vector<Config::DetectorPlacement>, size_t> m_detectors {};
    std::vector<Config::AtmosphereLayer> m_atmosphere_layers {};
    G4double m_world_size {};
    G4double m_atmosphere_height {};
    Config::MagneticField m_magnetic_field {};
    Config::DetectorProperties m_detector_properties {};
    G4double m_theta { 0 * radian };
    G4double m_phi { 0 * radian };

    struct
    {
        G4double x { 0 * m };
        G4double y { 0 * m };
        G4double z { 0 * m };
    } m_offset_bottom {};

    G4LogicalVolume* m_world_logical { nullptr };
    G4LogicalVolume* m_air_logical {};

    std::vector<G4LogicalVolume*> m_logical_volumes;
    std::vector<G4VPhysicalVolume*> m_physical_layers;
    std::vector<G4VSolid*> m_solid_layers;

    G4VSolid* m_detector_geometry { nullptr };
    G4Material* m_detector_material { nullptr };
    std::vector<G4LogicalVolume*> m_detector_logicals {};
    G4VPhysicalVolume* m_intensity_catcher_physical { nullptr };
    G4LogicalVolume* m_intensity_catcher_logical { nullptr };

    auto construct_world() -> G4VPhysicalVolume*;
    void construct_atmosphere();
    void construct_magnetic_field();
    void construct_detectors();
    void define_detectors();

    void place_detector(const std::string& name, const G4double& x, const G4double& y, const G4double& z);
};
}

#endif // DETECTORCONSTRUCTION_H
