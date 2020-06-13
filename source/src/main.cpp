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
    std::cout<<"Possible parameters:\n\t-h\t\tprint this help\n\t-c <filename>\tuse the config file <filename>\n\t\tdefault: shower.cfg\n";
}

int main(int argc, char* argv[])
{
    std::string config_file = "shower.cfg";
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
    /*
    G4UIExecutive *ui = 0;
    if (argc == 1)
    {
        ui = new G4UIExecutive(argc, argv, "qt");
    }
*/
#ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager;
#else
    G4RunManager* runManager = new G4RunManager;
#endif

    SHOWER::ConfigManager configManager{config_file};

    std::vector<SHOWER::Config::AtmosphereLayer> atmosphere_layers = configManager.get_atmosphere_layers();

    double atmosphere_upper = atmosphere_layers[atmosphere_layers.size() - 1].upper;


    runManager->SetUserInitialization(new SHOWER::DetectorConstruction(configManager.get_detectors(), atmosphere_layers, atmosphere_upper));


    G4VModularPhysicsList* physicsList = new QGSP_BERT;
    physicsList->SetVerboseLevel(1);

    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new SHOWER::ActionInitialization(configManager.get_primary_particle(), atmosphere_upper));

    runManager->Initialize();
/*
    G4VisManager* visManager = new G4VisExecutive;

    visManager->Initialize();


    if (!ui)
    {
        G4String command = "/control/execute";
        G4String fileName = argv[1];
        uiManager->ApplyCommand(command + fileName);
    }
    else
    {
        uiManager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }
    delete visManager;
    G4UImanager* uiManager = G4UImanager::GetUIpointer();
    uiManager->ApplyCommand("/run/verbose 1");
    uiManager->ApplyCommand("/event/verbose 1");
    uiManager->ApplyCommand("/tracking/verbose 1");
*/
    int numberOfEvent = 2000;
    runManager->BeamOn(numberOfEvent);

    delete runManager;
    return 0;
}
