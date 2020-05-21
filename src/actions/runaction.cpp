#include "actions/runaction.h"

#include "actions/primarygeneratoraction.h"
#include "detectorconstruction.h"

#include <G4RunManager.hh>
#include <G4Run.hh>
#include <G4AccumulableManager.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4LogicalVolume.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <g4csv.hh>

START_NAMESPACE
{
RunAction::RunAction() :
    G4UserRunAction(),
    m_edep(0),
    m_edep2(0)
{
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->RegisterAccumulable(m_edep);
    accumulableManager->RegisterAccumulable(m_edep2);
    
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->SetVerboseLevel(1);
    man->CreateH1("0", "Edep", 800, 0, 4*GeV);
    man->SetH1Plotting(0, true);
}

RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run*)
{
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);

    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Reset();

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->OpenFile("histogram");
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    if (man->GetH1(0))
    {
        G4cout<<"E_dep rms: "<<G4BestUnit(man->GetH1(0)->rms(), "Energy")<<G4endl;
    }
    man->Write();
    man->CloseFile();
/*
    G4int n_of_events = run->GetNumberOfEvent();
    if (n_of_events == 0)
    {
        return;
    }

    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Merge();

    G4double edep = m_edep.GetValue();
    G4double edep2 = m_edep2.GetValue();
    G4double rms = edep2 - edep*n_of_events;
    if (rms > 0)
    {
        rms = std::sqrt(rms);
    }
    else
    {
        rms = 0;
    }

    const DetectorConstruction* detector = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    G4double mass = detector->GetScoringVolume()->GetMass();
    G4double dose = edep/mass;
    G4double rmsDose = rms/mass;

    const PrimaryGeneratorAction* generator = static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    G4String runCondition;

    if (generator)
    {
        const G4ParticleGun* gun = generator->GetParticleGun();
        runCondition += gun->GetParticleDefinition()->GetParticleName();
        runCondition += " of ";
        G4double energy = gun->GetParticleEnergy();
        runCondition += G4BestUnit(energy, "Energy");
    }
    */
}

void RunAction::AddEdep(G4double edep)
{
    m_edep += edep;
    m_edep2 += edep*edep;

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillH1(0, edep);
    
}
}
