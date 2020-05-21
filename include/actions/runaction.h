#ifndef RUNACTION_H
#define RUNACTION_H

#include <G4UserRunAction.hh>
#include <G4Accumulable.hh>
#include <globals.hh>

#include "global.h"

class G4Run;

START_NAMESPACE
{
class RunAction : public G4UserRunAction
{
public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void AddEdep(G4double edep);

private:
    G4Accumulable<G4double> m_edep;
    G4Accumulable<G4double> m_edep2;
};
}

#endif // RUNACTION_H
