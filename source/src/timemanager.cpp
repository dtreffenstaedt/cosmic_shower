#include "timemanager.h"

#include <iostream>
#include <fstream>

START_NAMESPACE
{
TimeManager* TimeManager::c_singleton = nullptr;

TimeManager::Measurement::Measurement()
{}

TimeManager::Measurement::~Measurement()
{}

std::chrono::steady_clock::duration TimeManager::Measurement::stop()
{
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - m_start;

    m_promise.set_value(duration);

    return duration;
}

std::future<std::chrono::steady_clock::duration> TimeManager::Measurement::start()
{
    m_start = std::chrono::steady_clock::now();
    return m_promise.get_future();
}

TimeManager::TimeManager()
{
    c_singleton = this;
}

std::unique_ptr<TimeManager::Measurement> TimeManager::start(const std::string &id)
{
    std::unique_ptr<Measurement> measurement = std::make_unique<Measurement>();

    m_futures[id] =  measurement->start();

    return measurement;
}

void TimeManager::get_values()
{
    for (auto it = m_futures.begin(); it != m_futures.end(); it++)
    {
        m_values[it->first] = (it->second).get();
    }
}

void TimeManager::print_all()
{
    if (m_values.size() != m_futures.size())
    {
        get_values();
    }
    double size = m_values.size();
    double average = 0;
    double total = 0;
    for (auto it = m_values.begin(); it != m_values.end(); it++)
    {
        double v = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>((it->second)).count();
        std::cout<<(it->first)<<": "<<v<<"ms\n";
        average += v / size;
        total += v;
    }
    std::cout<<"average: "<<average<<"ms\n";
    std::cout<<"total: "<<total<<"ms\n";
}

void TimeManager::save_average(const std::string &first, const std::string& file)
{
    if (m_values.size() != m_futures.size())
    {
        get_values();
    }
    double size = m_values.size();
    double average = 0;
    for (auto it = m_values.begin(); it != m_values.end(); it++)
    {
        double v = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>((it->second)).count();
        average += v / size;
    }

    std::ofstream stream;

    stream.open(file, std::ofstream::app);

    stream<<first<<','<<std::to_string(average)<<'\n';

    stream.close();
}

TimeManager* TimeManager::singleton()
{
    return c_singleton;
}
}
