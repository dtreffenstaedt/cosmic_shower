#ifndef BENCHMARKMANAGER_H
#define BENCHMARKMANAGER_H

#include "global.h"

#include <chrono>
#include <string>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <fstream>

namespace Shower
{
class BenchmarkManager
{
public:
    class Measurement
    {
    public:
        Measurement(const std::string& filename, const std::string &name);
        virtual ~Measurement();

        std::chrono::steady_clock::duration stop();

        void start();

        void run();

    private:
        std::chrono::steady_clock::time_point m_start;
        std::chrono::steady_clock::duration m_duration;

        double m_virtual_memory{0};
        double m_physical_memory{0};
        size_t m_n{0};

        std::atomic<bool> m_stop{false};

        std::ofstream m_stream;

        int parse_line(char* line);

        int get_virtual_memory();
        int get_physical_memory();

        void end();
        std::unique_ptr<std::thread> m_thread;
    };

    BenchmarkManager(const std::string& filename);

    std::unique_ptr<Measurement> start(const std::string& id);

    static BenchmarkManager* singleton();



private:
    std::string m_file_name;


    static BenchmarkManager* c_singleton;


};
}

#endif // BENCHMARKMANAGER_H
