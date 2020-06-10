#include "actions/steppingaction.h"
#include "actions/eventaction.h"
#include "detectorconstruction.h"

#include <G4Step.hh>
#include <G4Event.hh>
#include <G4RunManager.hh>
#include <G4LogicalVolume.hh>
#include <g4csv.hh>

START_NAMESPACE
{
SteppingAction::SteppingAction(EventAction* eventAction) :
    G4UserSteppingAction(),
    m_eventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* s)
{
    /*
    if (!m_scoringVolume)
    {
        const DetectorConstruction* detector = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

        m_scoringVolume = detector->GetScoringVolume();
    }

    G4LogicalVolume* volume = s->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    if (volume != m_scoringVolume)
    {
        return;
    }
      */
    G4double edep_step = s->GetTotalEnergyDeposit();
    m_eventAction->AddEdep(edep_step);
}

}
