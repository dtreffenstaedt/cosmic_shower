#ifndef CORE_H
#define CORE_H

#include "global.h"

#include <string>

#include <G4RunManager.hh>
#include "configuration.h"
#include "parameters.h"
#include "recorder.h"

namespace Shower {
class Core {
public:
    Core(int argc, char* argv[]);

    virtual ~Core();

    auto execute() -> int;

private:
    void setup();

    G4RunManager* m_run_manager;


    std::shared_ptr<Configuration> m_configuration;
    std::shared_ptr<Parameters> m_parameters;
    std::shared_ptr<Recorder> m_recorder;
};
}
#endif // CORE_H
