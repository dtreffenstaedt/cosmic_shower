#ifndef EVENTACTION_H
#define EVENTACTION_H

#include <G4UserEventAction.hh>
#include <globals.hh>


#include "global.h"

START_NAMESPACE
{
class RunAction;

class EventAction : public G4UserEventAction
{
public:
    EventAction(RunAction* runAction);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* e);
    virtual void EndOfEventAction(const G4Event* e);

    void AddEdep(G4double edep);
private:
    RunAction*  m_runAction;
    G4double    m_edep;
};
}

#endif // EVENTACTION_H
