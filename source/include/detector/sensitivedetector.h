#ifndef SENSITIVEDETECTOR_H
#define SENSITIVEDETECTOR_H

#include "global.h"

#include <G4LogicalVolume.hh>
#include <G4THitsCollection.hh>
#include <G4VSensitiveDetector.hh>

namespace Shower {

class Recorder;

class SensitiveDetector : public G4VSensitiveDetector {
public:
    explicit SensitiveDetector(const std::string& name, std::shared_ptr<Recorder> recorder);

    ~SensitiveDetector() override = default;

    // +++ reimplemented from G4VSensitiveDetector
    auto ProcessHits(G4Step* step, G4TouchableHistory*) -> G4bool override;
    // --- reimplemented from G4VSensitiveDetector

private:
    std::shared_ptr<Recorder> m_recorder;
};

}
#endif // SENSITIVEDETECTOR_H
