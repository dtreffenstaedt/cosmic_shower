#include "node.h"

#include <string>
#include <iostream>
#include <filesystem>

#include <libconfig.h++>

#include "particledistributor.h"



auto main(const int argc, const char* argv[]) -> int
{
    if (argc != 2) {
        std::cout << "Need parameter to config file.\n";
        return -1;
    }
    std::string configfile { argv[1] };
    std::string secondaryfile {};
    libconfig::Config config;
    config.readFile(configfile.c_str());
    libconfig::Setting& root = config.getRoot();
    std::string name {};
    root.lookupValue("name", name);

    root.lookupValue("data_directory", secondaryfile);
    secondaryfile += "/" + name + "/event_1/secondaries";
    if (system(("./run -c " + configfile).c_str()) != 0) {
        std::cerr << "Error running simulation!\n";
    }

    if (std::filesystem::exists(secondaryfile)) {
        std::cout << "Starting sub simulations\n";
        Node::ParticleDistributor distributor { "./sims/", configfile, secondaryfile };
        distributor.distribute();
    } else {
        std::cout << " simulation is done\n";
    }
    return 0;
}
