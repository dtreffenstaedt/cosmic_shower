#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "global.h"

#include <chrono>
#include <string>
#include <future>
#include <map>
#include <memory>

START_NAMESPACE
{
class TimeManager
{
public:
    class Measurement
    {
    public:
        Measurement();
        virtual ~Measurement();

        std::chrono::steady_clock::duration stop();

        std::future<std::chrono::steady_clock::duration> start();

    private:
        std::chrono::steady_clock::time_point m_start;
        std::promise<std::chrono::steady_clock::duration> m_promise;
    };

    TimeManager();

    std::unique_ptr<Measurement> start(const std::string& id);

    void print_all();

    static TimeManager* singleton();

    void save_average(const std::string& first, const std::string& file = "data.csv");

private:
    void get_values();

    std::map<std::string, std::future<std::chrono::steady_clock::duration>> m_futures;
    std::map<std::string, std::chrono::steady_clock::duration> m_values;

    static TimeManager* c_singleton;
};
}

#endif // TIMEMANAGER_H
