#ifndef CORE_H
#define CORE_H

#include "global.h"

#include <string>


#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>

#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif
#include "configmanager.h"

START_NAMESPACE
{
class Core
{
public:
    Core(int argc, char* argv[]);

    virtual ~Core();

    int execute();

private:
    void setup(int argc, char *argv[]);
    void print_help() const;

    int execute_ui();
    int execute_cli();


#ifdef G4MULTITHREADED
    G4MTRunManager* m_run_manager;
#else
    G4RunManager* m_run_manager;
#endif

    G4VisManager* m_vis_manager;
    G4UIExecutive* m_ui_executive;
    G4UImanager* m_ui_manager;

    ConfigManager* m_config_manager;

};
}
#endif // CORE_H
