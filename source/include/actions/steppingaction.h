#ifndef STEPPINGACTION_H
#define STEPPINGACTION_H

#include "global.h"

#include <G4SystemOfUnits.hh>

#include <G4Types.hh>
#include <G4UserSteppingAction.hh>

class G4Step;

namespace Shower {

class Configuration;
class Recorder;
class CancelCriterion;

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(std::shared_ptr<Recorder> recorder, std::shared_ptr<CancelCriterion> cancel_criterion, const std::shared_ptr<Configuration>& configuration);

    void UserSteppingAction(const G4Step* step) override;

private:
    G4double m_limit { 0 * MeV };
    std::shared_ptr<Recorder> m_recorder;
    std::shared_ptr<CancelCriterion> m_cancel_criterion;
};
}

#endif // STEPPINGACTION_H
