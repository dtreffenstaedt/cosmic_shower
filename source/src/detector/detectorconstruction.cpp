#include "detector/detectorconstruction.h"

#include "detector/detailedsensitivedetector.h"
#include "detector/sensitivedetector.h"

#include <G4Box.hh>
#include <G4FieldManager.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4Para.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4TransportationManager.hh>
#include <G4UniformMagField.hh>
#include <G4UnitsTable.hh>
#include <G4UserLimits.hh>

namespace Shower {
DetectorConstruction::DetectorConstruction(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration)
    : m_recorder { recorder }
    , m_configuration { configuration }
    , m_detectors { configuration->get_detectors() }
    , m_atmosphere_layers { configuration->get_atmosphere_layers() }
    , m_world_size { configuration->get_world_size() }
    , m_atmosphere_height { configuration->get_atmosphere_height() }
    , m_magnetic_field { configuration->get_magnetic_field() }
    , m_detector_properties { configuration->get_detector_properties() }
    , m_offset_bottom { 0 * m, 0 * m, -m_atmosphere_height * 0.5 }
{
}

DetectorConstruction::~DetectorConstruction()
    = default;

auto DetectorConstruction::Construct() -> G4VPhysicalVolume*
{
    G4VPhysicalVolume* physical_world = construct_world();

    construct_atmosphere();

    construct_detectors();

    G4VSolid* intensity_catcher_solid = new G4Para { "Intensitycatcher", (m_world_size * 0.5), (m_world_size * 0.5), 200 * m, 0 * radian, m_theta, m_phi };

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* intensity_catcher_material = nist->FindOrBuildMaterial("G4_Pb");

    G4double offset_z = (-110 * m) + m_offset_bottom.z;

    m_intensity_catcher_logical = new G4LogicalVolume { intensity_catcher_solid, intensity_catcher_material, "Intensitycatcher" };
    m_intensity_catcher_physical = new G4PVPlacement {
        nullptr,
        G4ThreeVector(
            0 * m,
            0 * m,
            offset_z),
        m_intensity_catcher_logical,
        "Intensitycatcher",
        m_world_logical,
        false,
        0,
        true
    };
    return physical_world;
}

void DetectorConstruction::ConstructSDandField()
{
    define_detectors();

    construct_magnetic_field();
}

auto DetectorConstruction::construct_world() -> G4VPhysicalVolume*
{
    G4VSolid* world_solid = new G4Para("World", m_world_size * 0.5 + 100 * m, m_world_size * 0.5 + 100 * m, m_atmosphere_height * 0.5 + 100 * m, 0 * radian, m_theta, m_phi);

    auto* vacuum = new G4Material( // world is a vacuum
        "interGalactic",
        1.,
        1.008 * g / mole,
        1.e-25 * g / cm3,
        kStateGas,
        2.73 * kelvin,
        3.e-18 * pascal);

    m_world_logical = new G4LogicalVolume(world_solid, vacuum, "World");

    G4VPhysicalVolume* physical_world = new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        m_world_logical,
        "World",
        nullptr,
        false,
        0,
        true);

    Config::TrackingCuts cuts = m_configuration->get_tracking_cut();
    auto* limits = new G4UserLimits(DBL_MAX, DBL_MAX, DBL_MAX, cuts.energy, cuts.range);
    m_world_logical->SetUserLimits(limits);

    return physical_world;
}

void DetectorConstruction::construct_atmosphere()
{
    if (m_world_logical == nullptr) {
        return;
    }
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    size_t len = m_atmosphere_layers.size();
    for (size_t i = 0; i < len; i++) {
        Config::AtmosphereLayer definition = m_atmosphere_layers[i];
        G4double layer_lower = definition.lower; // lower end of each layer
        G4double layer_thickness = (definition.upper - definition.lower); // layer thickness
        G4double density = definition.density;
        G4double pressure = definition.pressure;
        G4double temperature = definition.temperature;

        auto* layer_material = new G4Material("atmosphere" + std::to_string(i), density, air, kStateGas, temperature, pressure);
        m_solid_layers.push_back(new G4Para("atmosphere" + std::to_string(i), m_world_size * 0.5, m_world_size * 0.5, layer_thickness * 0.5, 0 * radian, m_theta, m_phi));
        m_logical_volumes.push_back(new G4LogicalVolume(m_solid_layers[i], layer_material, "atmosphere" + std::to_string(i)));
        m_physical_layers.push_back(new G4PVPlacement(
            nullptr,
            G4ThreeVector(
                0 * m,
                0 * m,
                (layer_lower + layer_thickness * 0.5) + m_offset_bottom.z // center point of each layer
                ),
            m_logical_volumes[i],
            "phys_layer" + std::to_string(i),
            m_world_logical,
            false,
            0,
            true));
    }
}

void DetectorConstruction::construct_magnetic_field()
{
    if (m_world_logical == nullptr) {
        return;
    }

    auto* magnetic_field = new G4UniformMagField(G4ThreeVector(m_magnetic_field.x, m_magnetic_field.y, m_magnetic_field.z));

    G4FieldManager* field_manager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    field_manager->SetDetectorField(magnetic_field);
    field_manager->CreateChordFinder(magnetic_field);
}

void DetectorConstruction::construct_detectors()
{
    if (m_world_logical == nullptr) {
        return;
    }
    std::vector<Config::DetectorPlacement> detectors;
    if (std::holds_alternative<size_t>(m_detectors)) {
        size_t n = std::get<size_t>(m_detectors);
        double root = std::sqrt(static_cast<double>(n));
        size_t k = 0;
        if (std::abs(root - std::floor(root)) <= std::numeric_limits<double>::epsilon()) // perfect square, create a uniform pattern.
        {
            G4double distance = m_world_size / G4double((root + 1));
            size_t row = static_cast<size_t>(std::sqrt(n));
            G4double pos_x = -m_world_size * 0.5;
            for (size_t i = 1; i <= row; i++) {
                pos_x += distance;
                G4double pos_y = -m_world_size * 0.5;
                for (size_t j = 1; j <= row; j++) {
                    pos_y += distance;
                    place_detector("detector" + std::to_string(k), pos_x + m_offset_bottom.x, pos_y + m_offset_bottom.y, m_offset_bottom.z);
                    k++;
                }
            }
        } else // not a square, place detectors on circle
        {
            size_t max = (n % 2 == 0) ? n : (n - 1);
            double angle = std::atan(1.0) * 8 / (static_cast<double>(max) / 2.0);
            G4double r = m_world_size * 1 / 6;
            size_t m = max / 2 - (((max / 2) % 2 == 0) ? (0) : (1));
            for (size_t i = 1 - (n - max); i <= m; i++) {
                if (i == 0) {
                    place_detector("detector" + std::to_string(i), m_offset_bottom.x, m_offset_bottom.y, m_offset_bottom.z);
                    continue;
                }
                G4double x = std::cos(angle * static_cast<double>(i)) * r;
                G4double y = std::sin(angle * static_cast<double>(i)) * r;
                place_detector("detector" + std::to_string(i), x + m_offset_bottom.x, y + m_offset_bottom.y, m_offset_bottom.z);
            }
            r = m_world_size * 2 / 6;
            for (size_t i = m + 1; i <= max; i++) {
                G4double x = std::cos(angle * (static_cast<double>(i) - static_cast<double>(max) / 2.0 + 1.0)) * r;
                G4double y = std::sin(angle * (static_cast<double>(i) - static_cast<double>(max) / 2.0 + 1.0)) * r;
                place_detector("detector" + std::to_string(i), x + m_offset_bottom.x, y + m_offset_bottom.y, m_offset_bottom.z);
            }
        }
    } else {
        detectors = std::get<std::vector<Config::DetectorPlacement>>(m_detectors);
        for (auto placement : detectors) {
            place_detector(placement.name, (placement.x * m) + m_offset_bottom.x, (placement.y * m) + m_offset_bottom.y, (placement.z * m) + m_offset_bottom.z);
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

    m_configuration->add_detector(det);

    if (m_detector_geometry == nullptr) {
        m_detector_geometry = new G4Box("detector", m_detector_properties.geometry.x * 0.5, m_detector_properties.geometry.y * 0.5, m_detector_properties.geometry.z * 0.5);
    }
    if (m_detector_material == nullptr) {
        m_detector_material = new G4Material { "EJ-248", m_detector_properties.physical.z, m_detector_properties.physical.a, m_detector_properties.physical.rho, kStateSolid };
    }

    auto* logical = new G4LogicalVolume(m_detector_geometry, m_detector_material, name);

    new G4PVPlacement(
        nullptr,
        G4ThreeVector(
            x,
            y,
            z),
        logical,
        name,
        m_world_logical,
        false,
        0,
        true);

    m_detector_logicals.push_back(logical);
}

void DetectorConstruction::define_detectors()
{
    auto* detector = new DetailedSensitiveDetector { "detectors", m_recorder };
    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();
    sd_manager->AddNewDetector(detector);
    for (auto& m_detector_logical : m_detector_logicals) {
        m_detector_logical->SetSensitiveDetector(detector);
    }
    auto* detector_intensity = new SensitiveDetector { "intensity_catcher", m_recorder };
    sd_manager->AddNewDetector(detector_intensity);
    m_intensity_catcher_logical->SetSensitiveDetector(detector_intensity);
}
}
