#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>

#include <globals.hh>

#include <variant>

#include "global.h"
#include "configmanager.h"
#include <G4SystemOfUnits.hh>
#include <G4Box.hh>
#include <G4Material.hh>

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace Shower
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();

    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

protected:
    std::variant<std::vector<Config::DetectorPlacement>, size_t> m_detectors;
    std::vector<Config::AtmosphereLayer> m_atmosphere_layers;
    G4double m_world_size;
    G4double m_atmosphere_height;
    Config::MagneticField m_magnetic_field;
    Config::DetectorProperties m_detector_properties;
    Config::PrimaryParticle m_primary_particle;
    G4double m_theta;
    G4double m_phi;

    struct
    {
        G4double x;
        G4double y;
        G4double z;
    } m_offset_bottom;

    G4LogicalVolume* m_world_logical;
    G4LogicalVolume* m_air_logical;

    std::vector<G4LogicalVolume*> m_logical_volumes;
    std::vector<G4VPhysicalVolume*> m_physical_layers;
    std::vector<G4VSolid*> m_solid_layers;

    G4VSolid* m_detector_geometry;
    G4Material* m_detector_material;
    std::vector<G4LogicalVolume*> m_detector_logicals;

    G4VPhysicalVolume* construct_world();
    void construct_atmosphere();
    void construct_magnetic_field();
    void construct_detectors();
    void define_detectors();

    void place_detector(const std::string &name, const G4double& x, const G4double& y, const G4double &z);
};
}

#endif // DETECTORCONSTRUCTION_H
