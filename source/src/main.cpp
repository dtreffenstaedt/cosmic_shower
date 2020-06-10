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

int main()
{
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

    SHOWER::ConfigManager configManager{"cosmic_config.cfg"};

    runManager->SetUserInitialization(new SHOWER::DetectorConstruction(configManager.get_detectors()));


    G4VModularPhysicsList* physicsList = new QGSP_BERT;
    physicsList->SetVerboseLevel(1);

    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new SHOWER::ActionInitialization());
    
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
