#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>

#include <globals.hh>

#include <variant>

#include "global.h"
#include "configmanager.h"
#include <G4SystemOfUnits.hh>

class G4VPhysicalVolume;
class G4LogicalVolume;

START_NAMESPACE
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();

    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

protected:
    std::variant<std::vector<Config::DetectorPlacement>, size_t> m_detectors;
    std::vector<Config::AtmosphereLayer> m_atmosphere_layers;
    G4double m_world_size;
    G4double m_atmosphere_height;
    Config::MagneticField m_magnetic_field;

    G4LogicalVolume* m_world_logical;

    G4VPhysicalVolume* construct_world();
    void construct_atmosphere();
    void construct_magnetic_field();
    void construct_detectors();

    void place_detector(const std::string &name, const G4double& x, const G4double& y, const G4double &z);
};
}

#endif // DETECTORCONSTRUCTION_H
