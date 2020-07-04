#include "parametermanager.h"


#include <iostream>

START_NAMESPACE
{

ParameterManager* ParameterManager::c_singleton = nullptr;

ParameterManager::ParameterManager()
{
    c_singleton = this;
}

bool ParameterManager::start(int argc, char *argv[])
{
    if (argc < 2)
    {
        return true;
    }
    size_t len = m_arguments.size();
    for (int j = 1; j < argc; j++)
    {
        std::string arg = argv[j];
        bool found = false;
        for (size_t i = 0; i < len; i++)
        {
            if (arg.compare("-" + m_arguments[i].abbr) * arg.compare("--" + m_arguments[i].full) == 0)
            {
                if (m_arguments[i].has_value)
                {
                    if (++j >= argc)
                    {
                        std::cout<<"expected name after "<<arg<<"\n";
                        print_help();
                        return false;
                    }
                    m_arguments[i].value = argv[j];
                }
                std::cout<<"Found argument "<<arg<<'\n';
                m_arguments[i].is_set = true;
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::cout<<"Invalid argument "<<arg<<'\n';
        }
    }
    return true;
}

void ParameterManager::print_help() const
{
    std::cout<<"Possible arguments:\n";
    size_t len = m_arguments.size();
    for (size_t i = 0; i < len; i++)
    {
        std::cout<<"\t-"<<m_arguments[i].abbr<<"\t--"<<m_arguments[i].full;
        if (m_arguments[i].has_value)
        {
            std::cout<<" (value)";
        }
        std::cout<<"\n\t\t\t"<<m_arguments[i].description<<'\n';
    }
}

ParameterManager* ParameterManager::singleton()
{
    return c_singleton;
}

bool ParameterManager::argument_set(const std::string& name) const
{
    size_t len = m_arguments.size();
    for (size_t i = 0; i < len; i++)
    {
        if (name.compare(m_arguments[i].abbr) * name.compare(m_arguments[i].full) == 0)
        {
            return m_arguments[i].is_set;
        }
    }
    return false;
}

std::string ParameterManager::argument_value(const std::string& name) const
{
    size_t len = m_arguments.size();
    for (size_t i = 0; i < len; i++)
    {
        if (name.compare(m_arguments[i].abbr) * name.compare(m_arguments[i].full) == 0)
        {
            return m_arguments[i].value;
        }
    }
    return std::string{};
}

void ParameterManager::add_argument(const std::string& abbreviation, const std::string& full, const std::string &description, const bool& value)
{
    Commandline cmd;
    cmd.abbr = abbreviation;
    cmd.full = full;
    cmd.description = description;
    cmd.has_value = value;

    m_arguments.push_back(cmd);
}

}
