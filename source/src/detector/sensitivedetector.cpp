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

void SensitiveDetector::Initialize(G4HCofThisEvent* hit_collection)
{
    std::cout<<m_name<<" initialising event\n";

    m_hits_collection = new G4THitsCollection<DetectorHit>(GetName(), m_name);


    if (m_collection_id < 0)
    {
        m_collection_id = G4SDManager::GetSDMpointer()->GetCollectionID(m_hits_collection);
    }


    hit_collection->AddHitsCollection(m_collection_id, m_hits_collection);
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    std::cout<<m_name<<" got a hit!\n";
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


    if (m_hits_collection)
    {
        DetectorHit* hit = new DetectorHit{
                                    particle,
                                    e_tot,
                                    volume,
                                    position,
                                    momentum_direction,
                                    global_time
                                    };

        m_hits_collection->insert(hit);
    }


    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    std::cout<<m_name<<" ending event\n";
    if (!m_hits_collection)
    {
        return;
    }
    std::ofstream stream;
    size_t size = m_hits_collection->GetSize();
    std::cout<<m_name<<" had "<<std::to_string(size)<<" hits\n";
    for (size_t i = 0; i < size; i++)
    {
        DetectorHit* hit = static_cast<DetectorHit*>(m_hits_collection->GetHit(i));

        if (std::find(std::begin(m_pdg_codes), std::end(m_pdg_codes), hit->get_pdg()) == std::end(m_pdg_codes))
        {
            std::cout<<"Hit not in particle list. discarding ("<<std::to_string(hit->get_pdg())<<")\n";
            continue;
        }


        std::cout<<m_name<<" saving hit. ("<<std::to_string(hit->get_pdg())<<")\n";
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
}
