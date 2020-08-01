#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "global.h"

#include <string>
#include <vector>

namespace Shower {
class Parameters {
public:
    Parameters();

    [[nodiscard]] auto argument_set(const std::string& name) const -> bool;
    [[nodiscard]] auto argument_value(const std::string& name) const -> std::string;

    void add_argument(const std::string& abbreviation, const std::string& full, const std::string& description, const bool& value = false);

    auto start(int argc, char* argv[]) -> bool;

    void print_help() const;

private:
    struct Commandline {
        std::string abbr = std::string {};
        std::string full = std::string {};
        std::string description = std::string {};
        std::string value = std::string {};
        bool has_value = false;
        bool is_set = false;
    };

    std::vector<Commandline> m_arguments;
};
}
#endif // PARAMETERS_H
