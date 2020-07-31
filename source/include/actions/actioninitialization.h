#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H

#include <G4VUserActionInitialization.hh>
#include <globals.hh>

#include "global.h"

namespace Shower {
class ActionInitialization : public G4VUserActionInitialization {
public:
    explicit ActionInitialization(const std::shared_ptr<Recorder>& recorder, const std::shared_ptr<Configuration>& configuration);

    void BuildForMaster() const override;
    void Build() const override;

private:
    std::shared_ptr<Recorder> m_recorder;
    std::shared_ptr<Configuration> m_configuration;
};
}

#endif // ACTIONINITIALIZATION_H
