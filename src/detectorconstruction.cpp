#include "detectorconstruction.h"

#include <G4RunManager.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
DetectorConstruction::DetectorConstruction() :
    G4VUserDetectorConstruction(),
    m_scoring_volume(0)
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

    G4double world_length = 100*m;
    G4double side_length = 0.25*m;



    G4Box* world = new G4Box("World", world_length, world_length, world_length);

    G4Box* detector = new G4Box("Detector", side_length, side_length, side_length);
    

    G4double atomicNumber = 1.;
    G4double massOfMole = 1.008*g/mole;
    G4double density = 1.e-25*g/cm3;
    G4double temperature = 2.73*kelvin;
    G4double pressure = 3.e-18*pascal;

    G4Material* vacuum = new G4Material(
            "interGalactic",
            atomicNumber,
            massOfMole,
            density,
            kStateGas,
            temperature,
            pressure);

    G4NistManager* nist = G4NistManager::Instance();

    G4Material* aluminium = nist->FindOrBuildMaterial("G4_Fe");

    G4LogicalVolume* worldLog = new G4LogicalVolume(world, vacuum, "World");
    G4LogicalVolume* detectorLog = new G4LogicalVolume(detector, aluminium, "Detector");

    G4VPhysicalVolume* worldPhys = new G4PVPlacement(
            0,
            G4ThreeVector(),
            worldLog,
            "World",
            0,
            false,
            0,
            true);

    G4VPhysicalVolume* detectorPhys = new G4PVPlacement(
            0,
            G4ThreeVector(),
            detectorLog,
            "Detector",
            worldLog,
            false,
            0,
            true);

    m_scoring_volume = detectorLog;

    return worldPhys;
}

G4LogicalVolume* DetectorConstruction::GetScoringVolume() const
{
    return m_scoring_volume;
}
}
