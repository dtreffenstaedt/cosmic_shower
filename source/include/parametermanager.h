#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include "global.h"

#include <string>
#include <vector>

START_NAMESPACE
{
class ParameterManager
{
public:
    ParameterManager();

    bool argument_set(const std::string& name) const;
    std::string argument_value(const std::string& name) const;

    void add_argument(const std::string& abbreviation, const std::string& full, const std::string& description, const bool& value = false);

    bool start(int argc, char* argv[]);

    void print_help() const;

    static ParameterManager* singleton();

private:

    static ParameterManager* c_singleton;


    struct Commandline
    {
        std::string abbr = std::string{};
        std::string full = std::string{};
        std::string description = std::string{};
        bool has_value = false;
        bool is_set = false;
        std::string value = std::string{};
    };

    std::vector<Commandline> m_arguments;
};
}
#endif // PARAMETERMANAGER_H
