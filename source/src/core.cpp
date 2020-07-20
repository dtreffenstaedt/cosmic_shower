#include "core.h"

#include <iostream>
#include <random>

#include "actions/actioninitialization.h"
#include "configmanager.h"
#include "detector/detectorconstruction.h"
#include "physics/physicslist.h"

namespace Shower {
Core::Core(int argc, char* argv[])
    : m_run_manager { nullptr }
    ,
#ifdef SHOWER_BUILD_UI
    m_vis_manager { nullptr }
    , m_ui_executive { nullptr }
    , m_ui_manager { nullptr }
    ,
#endif
    m_parameter_manager { new ParameterManager {} }
{
#ifdef SHOWER_BUILD_UI
    m_parameter_manager->add_argument("g", "graphical", "Show the user interface");
#endif
    m_parameter_manager->add_argument("o", "overwrite", "Overwrite existing simulation data");

    ParameterManager::singleton()->add_argument("c", "config", "Use the configuration file specified in the value", true);

    if (!m_parameter_manager->start(argc, argv)) {
        exit(0);
    }

    if (m_parameter_manager->argument_set("c")) {
        m_config_manager = new ConfigManager { m_parameter_manager->argument_value("c") };
    } else {
        m_config_manager = new ConfigManager {};
    }
#ifdef SHOWER_BENCHMARK
    m_benchmark_manager = new BenchmarkManager { "benchmark" + std::to_string(ConfigManager::singleton()->get_primary_particle().momentum.m / 1000) + "GeV-" };
#endif
    m_recorder_manager = new RecorderManager {};
#ifdef SHOWER_BUILD_UI
    setup(argc, argv);
#else
    setup();
#endif
}

Core::~Core()
{
    delete m_recorder_manager;

    delete m_run_manager;

#ifdef SHOWER_BUILD_UI
    delete m_vis_manager;

    delete m_ui_executive;

    delete m_ui_manager;

#endif
    delete m_config_manager;
#ifdef SHOWER_BENCHMARK
    delete m_benchmark_manager;
#endif
    delete m_parameter_manager;
}

auto Core::execute() -> int
{

#ifdef SHOWER_BUILD_UI
    if (m_parameter_manager->argument_set("g")) {
        m_ui_manager = G4UImanager::GetUIpointer();

        if (m_ui_executive != nullptr) {
            return execute_ui();
        }
    }
#endif
    return execute_cli();
}

#ifdef SHOWER_BUILD_UI
auto Core::execute_ui() -> int
{
    m_ui_manager->ApplyCommand("/run/initialize");
    m_ui_manager->ApplyCommand("/control/execute vis.mac");
    m_ui_executive->SessionStart();
    return 0;
}
#endif
auto Core::execute_cli() -> int
{
    m_run_manager->BeamOn(m_config_manager->get_events());
    return 0;
}

#ifdef SHOWER_BUILD_UI
void Core::setup(int argc, char* argv[])
#else
void Core::setup()
#endif
{

    std::cout << "Starting simulation for '" << m_config_manager->get_name() << "'\n";
#ifdef SHOWER_BUILD_UI
    if (m_parameter_manager->argument_set("g")) {
        m_ui_executive = new G4UIExecutive(argc, argv, "qt");
    }
#endif
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine {});

    std::random_device rd;
    std::mt19937::result_type initial_seed = rd() ^ ((std::mt19937::result_type)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + (std::mt19937::result_type)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

    std::mt19937 gen(initial_seed);
    std::uniform_int_distribution<G4long> distribution;

    constexpr size_t max = 4;

    std::array<G4long, max> seeds {};

    for (long& seed : seeds) {
        seed = distribution(gen);
    }

    CLHEP::HepRandom::setTheSeeds(seeds.data());

#ifdef G4MULTITHREADED
    m_run_manager = new G4MTRunManager {};
    m_run_manager->SetNumberOfThreads(8);
#else
    m_run_manager = new G4RunManager {};
#endif

    m_run_manager->SetVerboseLevel(0);

    m_run_manager->SetUserInitialization(new DetectorConstruction {});

    m_run_manager->SetUserInitialization(new PhysicsList {});

    m_run_manager->SetUserInitialization(new ActionInitialization {});

    m_run_manager->Initialize();

#ifdef SHOWER_BUILD_UI
    m_vis_manager = new G4VisExecutive {};

    m_vis_manager->SetVerboseLevel(0);

    m_vis_manager->Initialize();
#endif
}

void Core::print_help()
{
    std::cout << "Possible parameters:\n\t-h\t\tprint this help\n\t-c <filename>\tuse the config file <filename>\n\t\tdefault: shower.cfg\n\t-ui\t\tshow the graphical user interface\n";
}
}
