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
    ActionInitialization(const Config::PrimaryParticle& primary, const double& atmosphere_upper);
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
private:
    Config::PrimaryParticle m_primary;
    G4double m_atmosphere_upper;
};
}

#endif // ACTIONINITIALIZATION_H
