#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>
#include <globals.hh>

#include <variant>

#include "global.h"
#include "configmanager.h"

class G4VPhysicalVolume;
class G4VLogicalVolume;

START_NAMESPACE
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction(const std::variant<std::vector<DetectorPlacement>, size_t>& detectors, const std::vector<AtmosphereLayer>& atmosphere_layers);

    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

protected:
    std::variant<std::vector<DetectorPlacement>, size_t> m_detectors;
    std::vector<AtmosphereLayer> m_atmosphere_layers;
};
}

#endif // DETECTORCONSTRUCTION_H
