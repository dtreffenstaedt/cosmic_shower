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
    m_parameters { std::make_shared<Parameters>() }
{
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
    m_recorder = std::make_shared<Recorder>(m_configuration, m_parameters);
    setup();
}

Core::~Core()
{
    delete m_run_manager;
}

auto Core::execute() -> int
{
    m_run_manager->BeamOn(1);
    return 0;
}

void Core::setup()
{
    std::cout << "Starting simulation for '" << m_configuration->get_name() << "'\n";
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

    m_run_manager = new G4RunManager {};

    m_run_manager->SetVerboseLevel(0);

    m_run_manager->SetUserInitialization(new DetectorConstruction { m_recorder, m_configuration });

    auto cancel_criterion {std::make_shared<NeverCancel>()};

    m_run_manager->SetUserInitialization(new PhysicsList { m_recorder, cancel_criterion});

    m_run_manager->SetUserInitialization(new ActionInitialization { m_recorder, m_configuration, cancel_criterion });

    m_run_manager->Initialize();

}
}
