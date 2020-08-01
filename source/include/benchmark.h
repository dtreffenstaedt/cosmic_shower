#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "global.h"

#include <chrono>
#include <fstream>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <string>

namespace Shower {
class Benchmark {
public:
    class Measurement {
    public:
        Measurement(const std::string& filename, const std::string& name);
        virtual ~Measurement();

        std::chrono::steady_clock::duration stop();

        void start();

        void run();

    private:
        std::chrono::steady_clock::time_point m_start;
        std::chrono::steady_clock::duration m_duration;

        double m_virtual_memory { 0 };
        double m_physical_memory { 0 };
        size_t m_n { 0 };

        std::atomic<bool> m_stop { false };

        std::ofstream m_stream;

        static auto parse_line(char* line) -> size_t;

        auto get_virtual_memory() -> double;
        auto get_physical_memory() -> double;
        auto get_swap_memory() -> double;

        void end();
        std::unique_ptr<std::thread> m_thread;
    };

    Benchmark(std::string  filename);

    std::unique_ptr<Measurement> start(const std::string& id);

private:
    std::string m_file_name;
};
}

#endif // BENCHMARK_H
