#include "core.h"

#include <iostream>
#include "configmanager.h"
#include "detector/detectorconstruction.h"
#include "actions/actioninitialization.h"
#include "physics/physicslist.h"

#include <G4MTRunManager.hh>
START_NAMESPACE
{
Core::Core(int argc, char *argv[]) :
    m_run_manager{nullptr},
    m_vis_manager{nullptr},
    m_ui_executive{nullptr},
    m_ui_manager{nullptr},
    m_config_manager{new ConfigManager{}}
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
    if (m_run_manager)
    {
        delete m_run_manager;
    }
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
    if (m_config_manager)
    {
        delete  m_config_manager;
    }
}

int Core::execute()
{
    if (m_config_manager->argument_set("ui"))
    {
        m_ui_manager = G4UImanager::GetUIpointer();

        if (m_ui_executive)
        {
            return execute_ui();
        }
    }
    return execute_cli();
}

int Core::execute_ui()
{
    m_ui_manager->ApplyCommand("/run/initialize");
    m_ui_manager->ApplyCommand("/control/execute vis.mac");
    m_ui_executive->SessionStart();
    return 0;
}

int Core::execute_cli()
{
    m_run_manager->BeamOn(m_config_manager->get_events());
    return 0;
}

void Core::setup(int argc, char *argv[])
{

    std::cout<<"Starting simulation for '"<<m_config_manager->get_name()<<"'\n";
    if (m_config_manager->argument_set("ui"))
    {
        m_ui_executive = new G4UIExecutive(argc, argv, "qt");
    }

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

    m_vis_manager = new G4VisExecutive;

    m_vis_manager->SetVerboseLevel(0);

    m_vis_manager->Initialize();
}

void Core::print_help() const
{
    std::cout<<"Possible parameters:\n\t-h\t\tprint this help\n\t-c <filename>\tuse the config file <filename>\n\t\tdefault: shower.cfg\n\t-ui\t\tshow the graphical user interface\n";
}
}
