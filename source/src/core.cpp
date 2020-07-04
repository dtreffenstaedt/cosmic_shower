#include "core.h"

#include <iostream>
#include "configmanager.h"
#include "detector/detectorconstruction.h"
#include "actions/actioninitialization.h"
#include "physics/physicslist.h"

START_NAMESPACE
{
Core::Core(int argc, char *argv[]) :
    m_run_manager{nullptr},
#ifdef SHOWER_BUILD_UI
    m_vis_manager{nullptr},
    m_ui_executive{nullptr},
    m_ui_manager{nullptr},
#endif
#ifdef SHOWER_BENCHMARK
    m_time_manager{new TimeManager{}},
#endif
    m_parameter_manager{new ParameterManager{}}
{
    m_config_manager->add_argument("ui", "user-interface", "Show the user interface");
    m_config_manager->add_argument("o", "overwrite", "Overwrite existing simulation data");
    if (!m_config_manager->start(argc, argv))
    {
        exit(0);
    }
    setup(argc, argv);
}

Core::~Core()
{
#ifdef SHOWER_BENCHMARK
    m_time_manager->print_all();
    m_time_manager->save_average(std::to_string(ConfigManager::singleton()->get_primary_particle().momentum.m/1000));
#endif

    if (m_run_manager)
    {
        delete m_run_manager;
    }
#ifdef SHOWER_BUILD_UI
    if (m_vis_manager)
    {
        delete m_vis_manager;
    }
    if (m_ui_executive)
    {
        delete m_ui_executive;
    }
    if (m_ui_manager)
    {
        delete m_ui_manager;
    }
#endif
    delete  m_config_manager;
    delete  m_parameter_manager;
#ifdef SHOWER_BENCHMARK
    delete m_time_manager;
#endif
}

int Core::execute()
{

#ifdef SHOWER_BUILD_UI
    if (m_parameter_manager->argument_set("ui"))
    {
        m_ui_manager = G4UImanager::GetUIpointer();

        if (m_ui_executive)
        {
            return execute_ui();
        }
    }
#endif
    return execute_cli();
}

#ifdef SHOWER_BUILD_UI
int Core::execute_ui()
{
    m_ui_manager->ApplyCommand("/run/initialize");
    m_ui_manager->ApplyCommand("/control/execute vis.mac");
    m_ui_executive->SessionStart();
    return 0;
}
#endif
int Core::execute_cli()
{
    m_run_manager->BeamOn(m_config_manager->get_events());
    return 0;
}

#ifdef SHOWER_BUILD_UI
void Core::setup(int argc, char *argv[])
#else
void Core::setup()
#endif
{

    std::cout<<"Starting simulation for '"<<m_config_manager->get_name()<<"'\n";
#ifdef SHOWER_BUILD_UI
    if (m_parameter_manager->argument_set("ui"))
    {
        m_ui_executive = new G4UIExecutive(argc, argv, "qt");
    }
#endif
#ifdef G4MULTITHREADED
    m_run_manager = new G4MTRunManager;
#else
    m_run_manager = new G4RunManager;
#endif

    m_run_manager->SetVerboseLevel(0);


    m_run_manager->SetUserInitialization(new DetectorConstruction);


    m_run_manager->SetUserInitialization(new PhysicsList(0));

    m_run_manager->SetUserInitialization(new ActionInitialization);

    m_run_manager->Initialize();

#ifdef SHOWER_BUILD_UI
    m_vis_manager = new G4VisExecutive;

    m_vis_manager->SetVerboseLevel(0);

    m_vis_manager->Initialize();
#endif
}

void Core::print_help() const
{
    std::cout<<"Possible parameters:\n\t-h\t\tprint this help\n\t-c <filename>\tuse the config file <filename>\n\t\tdefault: shower.cfg\n\t-ui\t\tshow the graphical user interface\n";
}
}
