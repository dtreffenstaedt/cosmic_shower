#include "detectorconstruction.h"
#include "actioninitialization.h"

#ifdef G4MULTITHREADED
#include <G4MtRunManager.hh>
#else
#include <G4RunManager.hh>
#endif

#include <G4UImanager.hh>
#include <QGSP_BERT.hh>

#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>

#include <Randomize.hh>

#include "configmanager.h"

void print_help()
{
    std::cout<<"Possible parameters:\n\t-h\t\tprint this help\n\t-c <filename>\tuse the config file <filename>\n\t\tdefault: shower.cfg\n\t-ui\t\tshow the graphical user interface\n";
}

int main(int argc, char* argv[])
{
    std::string config_file = "shower.cfg";
    bool use_ui = false;
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg(argv[i]);
            if (arg.compare("-c") == 0)
            {
                if (++i >= argc)
                {
                    std::cout<<"expected name after -c\n";
                    print_help();
                    return 1;
                }
                config_file = std::string(argv[i]);
            }
            else if (arg.compare("-ui") == 0)
            {
                use_ui = true;
            }
            else if (arg.compare("-h") == 0)
            {
                print_help();
                return 0;
            }
            else
            {
                std::cout<<"Invalid parameter "<<arg<<"\n\n";
                print_help();
                return 1;
            }
        }
    }

    SHOWER::ConfigManager configManager{config_file};

    std::cout<<"Starting simulation for '"<<configManager.get_name()<<"'\n";
    G4UIExecutive *ui = 0;
    if (use_ui)
    {
        ui = new G4UIExecutive(argc, argv, "qt");
    }

#ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager;
#else
    G4RunManager* runManager = new G4RunManager;
#endif
    runManager->SetVerboseLevel(0);


    runManager->SetUserInitialization(new SHOWER::DetectorConstruction);


    G4VModularPhysicsList* physicsList = new QGSP_BERT;
    physicsList->SetVerboseLevel(0);

    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new SHOWER::ActionInitialization);

    runManager->Initialize();

    G4VisManager* visManager = new G4VisExecutive;

    visManager->SetVerboseLevel(0);

    visManager->Initialize();

    if (use_ui)
    {
        G4UImanager* uiManager = G4UImanager::GetUIpointer();

        if (!ui)
        {
            runManager->BeamOn(configManager.get_events());
        }
        else
        {
            uiManager->ApplyCommand("/run/initialize");
            uiManager->ApplyCommand("/control/execute vis.mac");
            ui->SessionStart();
            delete ui;
        }

    }
    else
    {
        runManager->BeamOn(configManager.get_events());
    }
    delete visManager;
    delete runManager;
    return 0;
}
