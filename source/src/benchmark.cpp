#include "benchmark.h"

#include <fstream>
#include <iostream>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

namespace Shower {

Benchmark::Measurement::Measurement(const std::string& filename, const std::string& name)
{
    m_stream.open(filename + name);
    m_stream << "time[ms],virtual memory[MB],physical memory[MB],swap[MB]\n";
}

Benchmark::Measurement::~Measurement()
{
    m_thread->join();
    if (m_stream.is_open()) {
        m_stop = true;
        end();
    }
}

std::chrono::steady_clock::duration Benchmark::Measurement::stop()
{
    m_duration = std::chrono::steady_clock::now() - m_start;

    m_stop = true;
    return m_duration;
}

void Benchmark::Measurement::start()
{
    m_start = std::chrono::steady_clock::now();
    m_thread = std::make_unique<std::thread>(&Benchmark::Measurement::run, this);
}

void Benchmark::Measurement::run()
{
    while (!m_stop) {
        auto duration = std::chrono::steady_clock::now() - m_start;
        double virtual_memory = get_virtual_memory() / 1024.0;
        double physical_memory = get_physical_memory() / 1024.0;
        double swap_memory = get_swap_memory() / 1024.0;
        m_stream << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration).count() << ',' << std::to_string(virtual_memory) << ',' << std::to_string(physical_memory) << ',' << std::to_string(swap_memory) << '\n'
                 << std::flush;
        m_virtual_memory += virtual_memory;
        m_physical_memory += physical_memory;
        m_n++;
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    end();
}

void Benchmark::Measurement::end()
{
    m_virtual_memory /= static_cast<double>(m_n);
    m_physical_memory /= static_cast<double>(m_n);
    m_stream << "stats:\nt=" << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(m_duration).count() << "ms\nv=" << m_virtual_memory << "KB\np=" << m_physical_memory << "KB\n";
    m_stream.close();
}

int Benchmark::Measurement::parse_line(char* line)
{
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p < '0' || *p > '9')
        p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

auto Benchmark::Measurement::get_virtual_memory() -> double
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    return result;
}

auto Benchmark::Measurement::get_physical_memory() -> double
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    return result;
}

auto Benchmark::Measurement::get_swap_memory() -> double
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmSwap:", 7) == 0) {
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    return result;
}

Benchmark::Benchmark(const std::string& filename)
    : m_file_name { filename }
{
}

std::unique_ptr<Benchmark::Measurement> Benchmark::start(const std::string& id)
{
    std::unique_ptr<Measurement> measurement = std::make_unique<Measurement>(m_file_name, id);

    measurement->start();

    return measurement;
}

}