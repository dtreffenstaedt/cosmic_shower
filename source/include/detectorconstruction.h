#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>
#include <globals.hh>

#include "global.h"

class G4VPhysicalVolume;
class G4VLogicalVolume;

START_NAMESPACE
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();

    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    G4LogicalVolume* GetScoringVolume() const;

protected:
    G4LogicalVolume* m_scoring_volume;
};
}

#endif // DETECTORCONSTRUCTION_H
