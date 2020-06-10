#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>
#include <globals.hh>

#include "global.h"
#include "configmanager.h"

class G4VPhysicalVolume;
class G4VLogicalVolume;

START_NAMESPACE
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction(const std::vector<DetectorSettings>& detectors);

    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

protected:
    std::vector<DetectorSettings> m_detectors;
};
}

#endif // DETECTORCONSTRUCTION_H
