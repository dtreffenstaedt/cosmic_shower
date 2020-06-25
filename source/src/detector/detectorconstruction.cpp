#include "detector/detectorconstruction.h"

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
    m_detector_properties{ConfigManager::singleton()->get_detector_properties()},
    m_world_logical{nullptr},
    m_detector_geometry{nullptr},
    m_detector_material{nullptr}
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    std::cout<<"Setting up world:\n";
    std::cout<<'\t'<<G4BestUnit(m_world_size, "Length")<<'\n';
    std::cout<<'\t'<<G4BestUnit(m_atmosphere_height, "Length")<<'\n';
    G4VPhysicalVolume* physical_world = construct_world();

    construct_atmosphere();


    construct_detectors();

    return physical_world;
}

void DetectorConstruction::ConstructSDandField()
{

    define_detectors();

    construct_magnetic_field();
}

G4VPhysicalVolume* DetectorConstruction::construct_world()
{
    G4Box* world_box = new G4Box("World", m_world_size * 0.5 + 100 * m, m_world_size * 0.5 + 100 * m, m_atmosphere_height * 0.5 + 100 * m);

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

    size_t len = m_atmosphere_layers.size();
    for (size_t i = 0; i < len; i++)
    {
        Config::AtmosphereLayer definition = m_atmosphere_layers[i];
        G4double layer_lower = definition.lower * m; // lower end of each layer
        G4double layer_thickness = (definition.upper - definition.lower) * m; // layer thickness
        G4double density = definition.density * kg/m3;
        G4double pressure = definition.pressure * pascal;
        G4double temperature = definition.temperature * kelvin;

        G4Material* layer_material = new G4Material("atmosphere" + std::to_string(i), density, air, kStateGas, temperature, pressure);
        m_box_layers.push_back(new G4Box("atmosphere" + std::to_string(i), m_world_size * 0.5, m_world_size * 0.5, layer_thickness * 0.5));
        m_logical_volumes.push_back(new G4LogicalVolume(m_box_layers[i], layer_material, "atmosphere" + std::to_string(i)));
        m_physical_layers.push_back(new G4PVPlacement(
                0,
                G4ThreeVector(
                    0.0 * m,
                    0.0 * m,
                    - m_atmosphere_height * 0.5 + (layer_lower + layer_thickness * 0.5) // center point of each layer
                    ),
                m_logical_volumes[i],
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
    std::cout<<"\tDetectors: ";
    std::vector<Config::DetectorPlacement> detectors;
    if (std::holds_alternative<size_t>(m_detectors))
    {
        size_t n = std::get<size_t>(m_detectors);
        std::cout<<"\n\t\tn = "<<std::to_string(n)<<'\n';
        double root = std::sqrt(static_cast<double>(n));
        size_t k = 0;
        if (std::abs(root - std::floor(root)) <= std::numeric_limits<double>::epsilon()) // perfect square, create a uniform pattern.
        {
            std::cout<<"n is a perfect scare, arranging in regular pattern.\n";
            G4double distance = m_world_size / G4double((root + 1));
            size_t row = std::sqrt(n);
            G4double pos_x = - m_world_size * 0.5;
            for (size_t i = 1; i <= row; i++)
            {
                pos_x += distance;
                G4double pos_y = - m_world_size * 0.5;
                for (size_t j = 1; j <= row; j++)
                {
                    pos_y += distance;
                    place_detector("detector" + std::to_string(k), pos_x, pos_y, - m_atmosphere_height * 0.5);
                    k++;
                }
            }
        }
        else // not a square, place detectors on circle
        {
            std::cout<<"n not a perfect scare, arranging in circular pattern.\n";
            size_t max = (n%2 == 0)?n:(n-1);
            double angle = std::atan(1.0)*8/static_cast<double>(max/2);
            G4double r = m_world_size * 1/6;
            size_t m = max/2 - (((max/2)%2 == 0)?(0):(1));
            for (size_t i = 1 - (n - max); i <= m; i++)
            {
                if (i == 0)
                {
                    place_detector("detector" + std::to_string(i), 0, 0, - m_atmosphere_height * 0.5);
                    continue;
                }
                G4double x = std::cos(angle * static_cast<double>(i)) * r;
                G4double y = std::sin(angle * static_cast<double>(i)) * r;
                place_detector("detector" + std::to_string(i), x, y, - m_atmosphere_height * 0.5);
            }
            r = m_world_size * 2/6;
            for (size_t i = m + 1; i <= max; i++)
            {
                G4double x = std::cos(angle * static_cast<double>(i - max/2 + 1)) * r;
                G4double y = std::sin(angle * static_cast<double>(i - max/2 + 1)) * r;
                place_detector("detector" + std::to_string(i), x, y, - m_atmosphere_height * 0.5);
            }
        }
    }
    else
    {
        detectors = std::get<std::vector<Config::DetectorPlacement>>(m_detectors);
        std::cout<<"\n\t\tn = "<<std::to_string(detectors.size())<<'\n';
        for (size_t i = 0; i < detectors.size(); i++)
        {
            Config::DetectorPlacement placement = detectors[i];
            place_detector(placement.name, (placement.x * m), (placement.y * m), (placement.z * m) - m_atmosphere_height * 0.5);
        }
    }
}

void DetectorConstruction::place_detector(const std::string& name, const G4double& x, const G4double& y, const G4double& z)
{
    Config::DetectorPlacement det;
    det.x = x;
    det.y = y;
    det.z = z;
    det.name = name;

    ConfigManager::singleton()->add_detector(det);
    G4NistManager* nist = G4NistManager::Instance();


    if (!m_detector_geometry)
    {
        m_detector_geometry = new G4Box("detector", m_detector_properties.geometry.x * 0.5 * mm, m_detector_properties.geometry.y * 0.5 * mm, m_detector_properties.geometry.z * 0.5 * mm);
    }
    if (!m_detector_material)
    {
        m_detector_material = nist->FindOrBuildMaterial("G4_Fe");
    }

    G4LogicalVolume* logical = new G4LogicalVolume(m_detector_geometry, m_detector_material, name);

    new G4PVPlacement(
                0,
                G4ThreeVector(
                    x,
                    y,
                    z),
                logical,
                name,
                m_world_logical,
                false,
                0,
                true
                );

    m_detector_logicals.push_back(logical);
}

void DetectorConstruction::define_detectors()
{
    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();
    for (size_t i = 0; i < m_detector_logicals.size(); i++)
    {

        SensitiveDetector* detector = new SensitiveDetector(m_detector_logicals[i]->GetName());

        sd_manager->AddNewDetector(detector);
        m_detector_logicals[i]->SetSensitiveDetector(detector);

    }

}
}
