#ifndef STEPPINGACTION_H
#define STEPPINGACTION_H

#include "configuration.h"
#include "global.h"
#include "recorder.h"

#include <G4SystemOfUnits.hh>

#include <G4Types.hh>
#include <G4UserSteppingAction.hh>

class G4Step;

namespace Shower {

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration);

    void UserSteppingAction(const G4Step* step) override;

private:
    G4double m_limit { 0 * MeV };
    std::shared_ptr<Recorder> m_recorder;
};
}

#endif // STEPPINGACTION_H
