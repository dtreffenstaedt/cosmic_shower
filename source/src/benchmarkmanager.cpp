#include "benchmarkmanager.h"

#include <iostream>
#include <fstream>

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

START_NAMESPACE
{
BenchmarkManager* BenchmarkManager::c_singleton = nullptr;

BenchmarkManager::Measurement::Measurement(const std::string& filename, const std::string& name)
{
    m_stream.open(filename + name);
    m_stream<<"virtual memory,physical memory\n";
}

BenchmarkManager::Measurement::~Measurement()
{
    if (m_stream.is_open())
    {
        m_stop = true;
        end();
    }
}

std::chrono::steady_clock::duration BenchmarkManager::Measurement::stop()
{
    m_duration = std::chrono::steady_clock::now() - m_start;

    m_stop = true;
    return m_duration;
}

std::future<void> BenchmarkManager::Measurement::start()
{
    m_start = std::chrono::steady_clock::now();
    return std::async(std::launch::async, &BenchmarkManager::Measurement::run, this);
}

void BenchmarkManager::Measurement::run()
{
    while (!m_stop)
    {
        int virtual_memory = get_virtual_memory();
        int physical_memory = get_physical_memory();
        m_stream<<std::to_string(virtual_memory)<<','<<std::to_string(physical_memory)<<'\n';
        m_virtual_memory += virtual_memory;
        m_physical_memory += physical_memory;
        m_n++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    end();
}

void BenchmarkManager::Measurement::end()
{
    m_virtual_memory /= static_cast<double>(m_n);
    m_physical_memory /= static_cast<double>(m_n);
    m_stream<<"stats:\nt="<<std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(m_duration).count()<<"\nv="<<m_virtual_memory<<"\np="<<m_physical_memory<<'\n';
    m_stream.close();
}

int BenchmarkManager::Measurement::parse_line(char* line)
{
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int BenchmarkManager::Measurement::get_virtual_memory()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    return result;
}

int BenchmarkManager::Measurement::get_physical_memory()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    return result;
}

BenchmarkManager::BenchmarkManager(const std::string &filename) :
    m_file_name{filename}
{
    c_singleton = this;
}

std::unique_ptr<BenchmarkManager::Measurement> BenchmarkManager::start(const std::string &id)
{
    std::unique_ptr<Measurement> measurement = std::make_unique<Measurement>(m_file_name, id);

    measurement->start();

    return measurement;
}

BenchmarkManager* BenchmarkManager::singleton()
{
    return c_singleton;
}

}
