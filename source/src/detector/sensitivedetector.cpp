#include "detector/sensitivedetector.h"

#include "configmanager.h"
#include <G4PVPlacement.hh>
#include <G4Box.hh>
#include <G4SystemOfUnits.hh>
#include <G4NistManager.hh>
#include <G4SDManager.hh>

#include <filesystem>

START_NAMESPACE
{

SensitiveDetector::SensitiveDetector(const std::string& name) :
    G4VSensitiveDetector{"/" + name},
    m_hits_collection{nullptr},
    m_collection_id{-1},
    m_name{name}
{
    collectionName.push_back(name); // has to be filled for the hit collection to register
    std::string data = ConfigManager::singleton()->get_data_directory();
    std::string dir = data + "/" + ConfigManager::singleton()->get_name();
    m_file_name = dir + "/" + name;

    if (!std::filesystem::exists(data))
    {
        std::filesystem::create_directory(data);
    }

    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directory(dir);
    }

    std::cout<<"setting up sensitive detector '"<<name<<"' with data file: "<<m_file_name<<"\n";
    auto particles = ConfigManager::singleton()->get_particles();

    for (size_t i = 0; i < particles.size(); i++)
    {
        m_pdg_codes.push_back(particles[i].pdg);
    }
}

SensitiveDetector::~SensitiveDetector()
{
}

void SensitiveDetector::Initialise(G4HCofThisEvent* hit_collection)
{
    m_hits_collection = new G4THitsCollection<DetectorHit>(GetName(), collectionName[0]);


    if (m_collection_id < 0)
    {
        m_collection_id = GetCollectionID(0);
    }

    hit_collection->AddHitsCollection(m_collection_id, m_hits_collection);
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector momentum_direction = step->GetPreStepPoint()->GetMomentumDirection();

    G4Track* track = step->GetTrack();

    G4ParticleDefinition* particle = track->GetDefinition();
    G4String particle_name = particle->GetParticleName();

    G4VPhysicalVolume* volume = touchable->GetVolume(0);

    G4String detector = touchable->GetVolume(0)->GetName();

    G4double e_tot = track->GetTotalEnergy();

    G4double global_time = track->GetGlobalTime();


    DetectorHit* hit = new DetectorHit{
                                particle,
                                e_tot,
                                volume,
                                position,
                                momentum_direction,
                                global_time
                                };

    m_hits_collection->insert(hit);

    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    std::ofstream stream;
    size_t size = m_hits_collection->GetSize();
    for (size_t i = 0; i < size; i++)
    {
        DetectorHit* hit = static_cast<DetectorHit*>(m_hits_collection->GetHit(i));

        if (std::find(std::begin(m_pdg_codes), std::end(m_pdg_codes), hit->get_pdg()) == std::end(m_pdg_codes))
        {
            continue;
        }

        if (!stream.is_open())
        {
            stream.open(m_file_name, std::ofstream::app);
        }

        hit->write_to_file(stream);
    }
    if (stream.is_open())
    {
        stream.close();
    }
}

void SensitiveDetector::construct(G4LogicalVolume* logical_volume, G4double x, G4double y, G4double z)
{
    G4VSolid* solid = new G4Box("detector_solid" + m_name, 0.1 * m, 0.1 * m, 0.1 * m);


    G4NistManager* nist = G4NistManager::Instance();
    G4Material* material = nist->FindOrBuildMaterial("G4_Pb");

    G4LogicalVolume* logical = new G4LogicalVolume(solid, material, "detector_logial" + m_name);

    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();

    sd_manager->AddNewDetector(this);
    logical->SetSensitiveDetector(this);

    new G4PVPlacement(
                0,
                G4ThreeVector(
                    x,
                    y,
                    z),
                logical,
                m_name,
                logical_volume,
                false,
                0,
                true
                );
}

}
