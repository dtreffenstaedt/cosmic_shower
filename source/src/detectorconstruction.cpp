#include "detectorconstruction.h"

#include <G4RunManager.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
DetectorConstruction::DetectorConstruction(const std::variant<std::vector<DetectorSettings>, size_t>& detectors) :
    G4VUserDetectorConstruction{},
    m_detectors{detectors}
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

    G4NistManager* nist = G4NistManager::Instance();
    G4double world_side = 40*km; // cube shaped world, side length
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
    G4double rho_0 = 1.225 * kg / m3; // density of air, standard atmosphere

    // +++ Construct the world

    G4Box* world_box = new G4Box("World", world_side * 0.5, world_side * 0.5, world_side * 0.5);

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


    G4LogicalVolume* logical_world = new G4LogicalVolume(world_box, vacuum, "World");

    G4VPhysicalVolume* physical_world = new G4PVPlacement(
            0,
            G4ThreeVector(),
            logical_world,
            "World",
            0,
            false,
            0,
            true);

    // --- Construct the world


    // +++ construct the atmostphere
    // It consists of 12 layers of varying thickness.
    // The specific thickness of each layer has been numerically calculated.
    
    G4double layers_meters[12] = {707.612*m, 1480.13*m, 2330.68*m, 3276.84*m, 4342.89*m, 5563.75*m, 6992.27*m, 8714.17*m, 10883*m, 13818.6*m, 18422.3*m, 40000*m}; // upper limit of each layer
    
    G4double layers_factor[12] = {0.958913,0.878348,0.797762,0.717148,0.636496,0.555787,0.474994,0.394064,0.312884,0.231178,0.147671,0.0312708}; // factors for the density of each layer
//    G4Material* material_layers[12]; // materials for each layer
//    G4VSolid* box_layer[12]; // geometry of each layer
//    G4LogicalVolume* logical_layer[12]; // logical volumes for each layer
    G4VPhysicalVolume* physical_layer[12]; // physical volumes for each layer

    

    for (size_t i = 0; i < 12; i++)
    {
        G4double layer_lower = ((i == 0)?(0.0*m):(layers_meters[i - 1])); // lower end of each layer
        G4double layer_thickness = layers_meters[i] - layer_lower; // layer thickness

        G4Material* layer_material = new G4Material("material_layer" + std::to_string(i), rho_0 * layers_factor[i], air);
        G4Box* layer_box = new G4Box("box_layer" + std::to_string(i), world_side * 0.5, world_side * 0.5, layer_thickness * 0.5);
        G4LogicalVolume* layer_logical = new G4LogicalVolume(layer_box, layer_material, std::to_string(i));
        physical_layer[i] = new G4PVPlacement(
                0,
                G4ThreeVector(
                    0.0 * m,
                    0.0 * m,
                    - world_side * 0.5 + (layer_lower + layer_thickness * 0.5) // center point of each layer 
                    ),
                layer_logical,
                "phys_layer" + std::to_string(i),
                logical_world,
                false,
                0,
                true
                );
    }

    // --- construct the atmostphere


    return physical_world;
}
}
