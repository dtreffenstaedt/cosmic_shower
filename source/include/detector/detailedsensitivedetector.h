#ifndef DETAILEDSENSITIVEDETECTOR_H
#define DETAILEDSENSITIVEDETECTOR_H

#include "global.h"

#include <G4LogicalVolume.hh>
#include <G4THitsCollection.hh>
#include <G4VSensitiveDetector.hh>

namespace Shower {

class Recorder;

class DetailedSensitiveDetector : public G4VSensitiveDetector {
public:
    explicit DetailedSensitiveDetector(const std::string& name, std::shared_ptr<Recorder> recorder);

    ~DetailedSensitiveDetector() override = default;

    // +++ reimplemented from G4VSensitiveDetector
    auto ProcessHits(G4Step* step, G4TouchableHistory*) -> G4bool override;
    // --- reimplemented from G4VSensitiveDetector

private:
    std::vector<int> m_pdg_codes;
    std::shared_ptr<Recorder> m_recorder;
};

}
#endif // DETAILEDSENSITIVEDETECTOR_H
