#include "actions/eventaction.h"
#include "actions/runaction.h"

#include <G4Event.hh>
#include <G4RunManager.hh>


START_NAMESPACE
{
EventAction::EventAction(RunAction* runAction) :
    G4UserEventAction(),
    m_runAction(runAction),
    m_edep(0)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    m_edep = 0;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    m_runAction->AddEdep(m_edep);
}

void EventAction::AddEdep(G4double edep)
{
    m_edep += edep;
}
}
