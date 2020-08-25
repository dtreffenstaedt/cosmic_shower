#include "node.h"

#include <filesystem>
#include <iostream>
#include <string>

#include <libconfig.h++>

#include "corerunner.h"
#include "runner.h"

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

    Node::CoreRunner core { config_dir };

    core.register_instance(name);

    return core.run();
}
