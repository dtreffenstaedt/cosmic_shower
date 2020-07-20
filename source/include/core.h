#ifndef CORE_H
#define CORE_H

#include "global.h"

#include <string>

#ifdef SHOWER_BUILD_UI
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#endif

#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif
#include "configmanager.h"
#include "parametermanager.h"
#include "recordermanager.h"
#ifdef SHOWER_BENCHMARK
#include "benchmarkmanager.h"
#endif

namespace Shower {
class Core {
public:
    Core(int argc, char* argv[]);

    virtual ~Core();

    auto execute() -> int;

private:
#ifdef SHOWER_BUILD_UI
    void setup(int argc, char* argv[]);
#else
    void setup();
#endif
    static void print_help();

#ifdef SHOWER_BUILD_UI
    auto execute_ui() -> int;
#endif
    auto execute_cli() -> int;

#ifdef G4MULTITHREADED
    G4MTRunManager* m_run_manager;
#else
    G4RunManager* m_run_manager;
#endif

#ifdef SHOWER_BUILD_UI
    G4VisManager* m_vis_manager;
    G4UIExecutive* m_ui_executive;
    G4UImanager* m_ui_manager;
#endif

    ConfigManager* m_config_manager;
#ifdef SHOWER_BENCHMARK
    BenchmarkManager* m_benchmark_manager;
#endif
    ParameterManager* m_parameter_manager;
    RecorderManager* m_recorder_manager;
};
}
#endif // CORE_H
