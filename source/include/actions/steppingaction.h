#ifndef STEPPINGACTION_H
#define STEPPINGACTION_H

#include "global.h"

#include <G4UserSteppingAction.hh>
#include <G4Types.hh>

class G4Step;

namespace Shower
{

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction();
    virtual ~SteppingAction();

    void UserSteppingAction(const G4Step* step);

private:
    double m_limit;
    G4double m_primary_energy;
};
}

#endif // STEPPINGACTION_H
