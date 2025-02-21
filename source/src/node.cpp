#include "node.h"

#include <iostream>
#include <string>

#include <libconfig.h++>

#include "corerunner.h"

#include <filesystem>

auto main(const int argc, const char* argv[]) -> int
{
    if (argc != 3) {
        std::cout << "Need parameter to config directory and sim name.\n";
        return -1;
    }
    std::string config_dir { argv[1] };

    if (!std::filesystem::exists(config_dir)) {
        std::cout << "Invalid config dir '" << config_dir << "'\n";
    }

    std::string name { argv[2] };
    if (!std::filesystem::exists(config_dir + "/" + name)) {
        std::cout << "Invalid configuration name '" << name << "'\n";
        return 1;
    }

    Node::CoreRunner core { config_dir, name};

    return core.run();
}
