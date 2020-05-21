#ifndef STEPPINGACTION_H
#define STEPPINGACTION_H

#include <G4UserSteppingAction.hh>
#include <globals.hh>

#include "global.h"

class G4LogicalVolume;

START_NAMESPACE
{
class EventAction;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* s);
private:
    EventAction* m_eventAction;
    G4LogicalVolume *m_scoringVolume;
};
}

#endif // STEPPINGACTION_H
