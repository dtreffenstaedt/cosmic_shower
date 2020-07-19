#ifndef SENSITIVEDETECTOR_H
#define SENSITIVEDETECTOR_H

#include "detector/detectorhit.h"
#include "global.h"

#include <G4LogicalVolume.hh>
#include <G4THitsCollection.hh>
#include <G4VSensitiveDetector.hh>

namespace Shower {

class SensitiveDetector : public G4VSensitiveDetector {
public:
    explicit SensitiveDetector(const std::string& name);

    ~SensitiveDetector() override;

    // +++ reimplemented from G4VSensitiveDetector
    void Initialize(G4HCofThisEvent* hit_collection) override;
    auto ProcessHits(G4Step* step, G4TouchableHistory*) -> G4bool override;
    void EndOfEvent(G4HCofThisEvent* hit_collection) override;
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
