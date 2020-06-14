#include "detectorconstruction.h"

#include "detector/sensitivedetector.h"

#include <G4RunManager.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UniformMagField.hh>
#include <G4TransportationManager.hh>
#include <G4FieldManager.hh>
#include <G4UnitsTable.hh>
#include <G4SDManager.hh>

START_NAMESPACE
{
DetectorConstruction::DetectorConstruction() :
    G4VUserDetectorConstruction{},
    m_detectors{ConfigManager::singleton()->get_detectors()},
    m_atmosphere_layers{ConfigManager::singleton()->get_atmosphere_layers()},
    m_world_size{ConfigManager::singleton()->get_world_size() * m},
    m_atmosphere_height{ConfigManager::singleton()->get_atmosphere_height() * m},
    m_magnetic_field{ConfigManager::singleton()->get_magnetic_field()},
    m_world_logical{nullptr},
    m_detector_geometry{nullptr},
    m_detector_material{nullptr}
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4VPhysicalVolume* physical_world = construct_world();

    construct_atmosphere();

    construct_magnetic_field();

    construct_detectors();

    return physical_world;
}

G4VPhysicalVolume* DetectorConstruction::construct_world()
{
    // +++ Construct the world

    G4Box* world_box = new G4Box("World", m_world_size * 0.5, m_world_size * 0.5, m_atmosphere_upper * 0.5);

    G4Material* vacuum = new G4Material( // world is a vacuum
            "interGalactic",
            1.,
            1.008 * g / mole,
            1.e-25 * g / cm3,
            kStateGas,
            2.73 * kelvin,
            3.e-18 * pascal);


    m_world_logical = new G4LogicalVolume(world_box, vacuum, "World");

    G4VPhysicalVolume* physical_world = new G4PVPlacement(
            0,
            G4ThreeVector(),
            m_world_logical,
            "World",
            0,
            false,
            0,
            true);

    return physical_world;
}

void DetectorConstruction::construct_atmosphere()
{
    if (!m_world_logical)
    {
        return;
    }
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    // +++ construct the atmostphere
    // It consists of 12 layers of varying thickness.
    // The specific thickness of each layer has been numerically calculated.

    std::vector<G4VPhysicalVolume*> physical_layers; // physical volumes for each layer



    size_t len = m_atmosphere_layers.size();
    for (size_t i = 0; i < len; i++)
    {
        Config::AtmosphereLayer definition = m_atmosphere_layers[i];
        G4double layer_lower = ((i == 0)?(0.0*m):(definition.lower * m)); // lower end of each layer
        G4double layer_thickness = (definition.upper - definition.lower) * m; // layer thickness
        G4double density = definition.density * kg/m3;
        G4double pressure = definition.pressure * pascal;
        G4double temperature = definition.temperature * kelvin;

        G4Material* layer_material = new G4Material("material_layer" + std::to_string(i), density, air, kStateUndefined, temperature, pressure);
        G4Box* layer_box = new G4Box("box_layer" + std::to_string(i), m_world_size * 0.5, m_world_size * 0.5, layer_thickness * 0.5);
        G4LogicalVolume* layer_logical = new G4LogicalVolume(layer_box, layer_material, std::to_string(i));
        physical_layers.push_back(new G4PVPlacement(
                0,
                G4ThreeVector(
                    0.0 * m,
                    0.0 * m,
                    - m_world_size * 0.5 + (layer_lower + layer_thickness * 0.5) // center point of each layer
                    ),
                layer_logical,
                "phys_layer" + std::to_string(i),
                m_world_logical,
                false,
                0,
                true
                ));
    }
}

void DetectorConstruction::construct_magnetic_field()
{
    if (!m_world_logical)
    {
        return;
    }

    G4double microTesla = tesla / 1000000.0;

    std::cout<<"\t Magnetic field:\n\t\tB_x = "<<m_magnetic_field.x<<"\n\t\tB_y = "<<m_magnetic_field.y<<"\n\t\tB_z = "<<m_magnetic_field.z<<'\n';

    G4UniformMagField *magnetic_field = new G4UniformMagField(G4ThreeVector(m_magnetic_field.x * microTesla, m_magnetic_field.y * microTesla, m_magnetic_field.z * microTesla));

    G4FieldManager *field_manager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    field_manager->SetDetectorField(magnetic_field);
    field_manager->CreateChordFinder(magnetic_field);
}

void DetectorConstruction::construct_detectors()
{
    if (!m_world_logical)
    {
        return;
    }
    std::vector<Config::DetectorPlacement> detectors;
    if (std::holds_alternative<size_t>(m_detectors))
    {
        size_t n = std::get<size_t>(m_detectors);
        double root = std::sqrt(static_cast<double>(n));
        if (std::abs(root - std::floor(root)) <= std::numeric_limits<double>::epsilon()) // perfect square, create a uniform pattern.
        {
            G4double distance = m_world_size / G4double((root + 1));
            size_t row = std::sqrt(n);
            G4double pos_x = 0 * m;
            for (size_t i = 0; i < row; i++)
            {
                pos_x += distance;
                G4double pos_y = 0 * m;
                for (size_t j = 0; j < row; j++)
                {
                    pos_y += distance;
                    place_detector("detector" + std::to_string(i*j), pos_x, pos_y, - m_atmosphere_upper * 0.5);
                }
            }
        }
        else // not a square, place detectors on circle
        {
            double angle = std::atan(1.0)*8/static_cast<double>(n);
            G4double r = m_world_size * 0.25;
            for (size_t i = 1; i <= n; i++)
            {
                G4double x = std::cos(angle * static_cast<double>(i)) * r + m_world_size * 0.5;
                G4double y = std::sin(angle * static_cast<double>(i)) * r + m_world_size * 0.5;
                place_detector("detector" + std::to_string(i), x, y, - m_atmosphere_upper * 0.5);
            }
        }
    }
    else
    {
        detectors = std::get<std::vector<Config::DetectorPlacement>>(m_detectors);
        for (size_t i = 0; i < detectors.size(); i++)
        {
            Config::DetectorPlacement placement = detectors[i];
            place_detector(placement.name, (placement.x * m), (placement.y * m), (placement.z * m) - (m_atmosphere_upper * 0.5));
        }
    }
}

void DetectorConstruction::place_detector(const std::string& name, const G4double& x, const G4double& y, const G4double& z)
{
    SensitiveDetector* detector = new SensitiveDetector(name);

    detector->construct(m_world_logical, x, y, z);
}
}
