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
    virtual void Initialize(G4HCofThisEvent* hit_collection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
    virtual void EndOfEvent(G4HCofThisEvent* hit_collection);
    // --- reimplemented from G4VSensitiveDetector

private:
    G4THitsCollection<DetectorHit>* m_hits_collection;
    G4int m_collection_id;
    std::string m_file_name;
    std::string m_name;
    std::vector<int> m_pdg_codes;
    size_t m_event_no;
    static std::atomic<bool> c_dumped;
};

}
#endif // SENSITIVEDETECTOR_H
