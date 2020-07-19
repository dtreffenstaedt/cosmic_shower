#ifndef STEPPINGACTION_H
#define STEPPINGACTION_H

#include "configmanager.h"
#include "global.h"
#include <G4SystemOfUnits.hh>

#include <G4Types.hh>
#include <G4UserSteppingAction.hh>

class G4Step;

namespace Shower {

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction();
    ~SteppingAction() override;

    void UserSteppingAction(const G4Step* step) override;

private:
    double m_limit { ConfigManager::singleton()->get_initial_ratio() };
    G4double m_primary_energy { ConfigManager::singleton()->get_primary_particle().momentum.m * MeV };
};
}

#endif // STEPPINGACTION_H
