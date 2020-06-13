#ifndef SENSITIVEDETECTOR_H
#define SENSITIVEDETECTOR_H

#include "global.h"
#include "detector/detectorhit.h"

#include <G4VSensitiveDetector.hh>
#include <G4THitsCollection.hh>
#include <G4LogicalVolume.hh>

START_NAMESPACE
{

class SensitiveDetector : public G4VSensitiveDetector
{
public:
    SensitiveDetector(const std::string& name);

    virtual ~SensitiveDetector();

    // +++ reimplemented from G4VSensitiveDetector
    virtual void Initialise(G4HCofThisEvent* hit_collection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
    virtual void EndOfEvent(G4HCofThisEvent* hit_collection);
    // --- reimplemented from G4VSensitiveDetector

    void construct(G4LogicalVolume* logical_volume, G4double x, G4double y, G4double z);

private:
    G4THitsCollection<DetectorHit>* m_hits_collection;
    G4int m_collection_id;
    std::string m_file_name;
    std::string m_name;
    std::vector<int> m_pdg_codes;
};

}
#endif // SENSITIVEDETECTOR_H
