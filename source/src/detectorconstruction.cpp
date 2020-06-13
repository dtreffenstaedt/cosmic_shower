#include "detectorconstruction.h"

#include <G4RunManager.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>

START_NAMESPACE
{
DetectorConstruction::DetectorConstruction(const std::variant<std::vector<DetectorPlacement>, size_t>& detectors, const std::vector<AtmosphereLayer>& atmosphere_layers) :
    G4VUserDetectorConstruction{},
    m_detectors{detectors},
    m_atmosphere_layers{atmosphere_layers}
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

    G4NistManager* nist = G4NistManager::Instance();
    G4double world_side = 40 * km; // cube shaped world, side length
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    // +++ Construct the world

    G4Box* world_box = new G4Box("World", world_side * 0.5, world_side * 0.5, world_side * 0.5);

    G4Material* vacuum = new G4Material(
            "interGalactic",
            1.,
            1.008 * g / mole,
            1.e-25 * g / cm3,
            kStateGas,
            2.73 * kelvin,
            3.e-18 * pascal);


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
    
    std::vector<G4VPhysicalVolume*> physical_layers; // physical volumes for each layer

    

    size_t len = m_atmosphere_layers.size();
    for (size_t i = 0; i < len; i++)
    {
        AtmosphereLayer definition = m_atmosphere_layers[i];
        G4double layer_lower = ((i == 0)?(0.0*m):(definition.lower * m)); // lower end of each layer
        G4double layer_thickness = (definition.upper - definition.lower) * m; // layer thickness
        G4double density = definition.density * kg/m3;
        G4double pressure = definition.pressure * pascal;
        G4double temperature = definition.temperature * kelvin;

        G4Material* layer_material = new G4Material("material_layer" + std::to_string(i), density, air, kStateUndefined, temperature, pressure);
        G4Box* layer_box = new G4Box("box_layer" + std::to_string(i), world_side * 0.5, world_side * 0.5, layer_thickness * 0.5);
        G4LogicalVolume* layer_logical = new G4LogicalVolume(layer_box, layer_material, std::to_string(i));
        physical_layers.push_back(new G4PVPlacement(
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
                ));
    }

    // --- construct the atmostphere


    return physical_world;
}
}
