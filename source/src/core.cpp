#include "core.h"

#include <iostream>
#include <random>

#include "actions/actioninitialization.h"
#include "detector/detectorconstruction.h"
#include "physics/physicslist.h"

#include "cancelcriterion.h"

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
    m_parameters { std::make_shared<Parameters>() }
{
#ifdef SHOWER_BUILD_UI
    m_parameters->add_argument("g", "graphical", "Show the user interface");
#endif
    m_parameters->add_argument("o", "overwrite", "Overwrite existing simulation data");

    m_parameters->add_argument("c", "config", "Use the configuration file specified in the value", true);

    if (!m_parameters->start(argc, argv)) {
        exit(0);
    }

    if (m_parameters->argument_set("c")) {
        m_configuration = std::make_shared<Configuration>(m_parameters->argument_value("c"));
    } else {
        m_configuration = std::make_shared<Configuration>();
    }
#ifdef SHOWER_BENCHMARK
    m_benchmark = std::make_shared<Benchmark>("benchmark");
#endif
    m_recorder = std::make_shared<Recorder>(m_configuration, m_parameters);
#ifdef SHOWER_BUILD_UI
    setup(argc, argv);
#else
    setup();
#endif
}

Core::~Core()
{
    m_recorder->save();
    delete m_run_manager;

#ifdef SHOWER_BUILD_UI
    delete m_vis_manager;

    delete m_ui_executive;

    delete m_ui_manager;

#endif
}

auto Core::execute() -> int
{

#ifdef SHOWER_BUILD_UI
    if (m_parameters->argument_set("g")) {
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
    m_run_manager->Initialize();
    m_ui_manager->ExecuteMacroFile("vis.mac");
    m_ui_executive->SessionStart();
    return 0;
}
#endif
auto Core::execute_cli() -> int
{
    m_run_manager->BeamOn(m_configuration->get_events());
    return 0;
}

#ifdef SHOWER_BUILD_UI
void Core::setup(int argc, char* argv[])
#else
void Core::setup()
#endif
{
    std::cout << "Starting simulation for '" << m_configuration->get_name() << "'\n";
#ifdef SHOWER_BUILD_UI
    if (m_parameters->argument_set("g")) {
        m_ui_executive = new G4UIExecutive(argc, argv, "qt");
    }
#endif
    //    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine {});

    std::random_device rd;
    std::mt19937::result_type initial_seed = rd() ^ (static_cast<std::mt19937::result_type>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()) + static_cast<std::mt19937::result_type>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()));

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

    m_run_manager->SetUserInitialization(new DetectorConstruction { m_recorder, m_configuration });

    auto cancel_criterion {std::make_shared<TimedCancel>(std::chrono::minutes { 30 })};

    m_run_manager->SetUserInitialization(new PhysicsList { m_recorder, cancel_criterion});

#ifdef SHOWER_BENCHMARK
    m_run_manager->SetUserInitialization(new ActionInitialization { m_recorder, m_configuration, m_benchmark, cancel_criterion });
#else
    m_run_manager->SetUserInitialization(new ActionInitialization { m_recorder, m_configuration, cancel_criterion });
#endif

    m_run_manager->Initialize();

#ifdef SHOWER_BUILD_UI
    m_vis_manager = new G4VisExecutive {};

    m_vis_manager->SetVerboseLevel(0);

    m_vis_manager->Initialize();
#endif
}
}
