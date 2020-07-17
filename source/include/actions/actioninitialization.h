#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H

#include <G4VUserActionInitialization.hh>
#include <globals.hh>

#include "global.h"

namespace Shower
{
class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization();
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
private:
};
}

#endif // ACTIONINITIALIZATION_H
