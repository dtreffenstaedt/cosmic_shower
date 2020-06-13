#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H

#include <G4VUserActionInitialization.hh>
#include <globals.hh>

#include "global.h"
#include "configmanager.h"

START_NAMESPACE
{
class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization(const PrimaryParticle& primary);
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
private:
    PrimaryParticle m_primary;
};
}

#endif // ACTIONINITIALIZATION_H
