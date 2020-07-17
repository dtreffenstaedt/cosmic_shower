#include "actions/steppingaction.h"

#include "configmanager.h"
#include "recordermanager.h"

#include <G4Track.hh>
#include <G4Step.hh>
#include <G4SystemOfUnits.hh>

#include <iostream>

namespace Shower
{

SteppingAction::SteppingAction() :
    G4UserSteppingAction{},
    m_limit{ConfigManager::singleton()->get_initial_ratio()},
    m_primary_energy{ConfigManager::singleton()->get_primary_particle().momentum.m * MeV}
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
    if (RecorderManager::singleton()->stored_primary())
    {
        return;
    }
    G4Track* track = step->GetTrack();

    if (track->GetParentID() != 0) // only check the primary track
    {
        return;
    }

    if (step->GetTotalEnergyDeposit() < (m_limit * m_primary_energy))
    {
        return;
    }
    std::cout<<"Storing initial interaction\n";
    RecorderManager::singleton()->store_primary(track->GetPosition(), track->GetGlobalTime());
}
}
