#include "bench/timer.hpp"

#include <chrono>
#include <thread>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)           
    #include <x86intrin.h>  
#endif

namespace hft::bench
{
    Cycles readCycles()
    {
        #if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
            unsigned int aux;
            return __rdtscp(&aux);    
        #elif defined(__aarch64__) || defined(__arm__)
            uint64_t virtual_timer_value;
            asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
            return virtual_timer_value;
        #else
            #error "readCycles() has not implemented for this architecture"
        #endif
    }

    void Clock::calibrate()
    {
        constexpr auto kCalibrationDuration = std::chrono::milliseconds(100);

        const Cycles start_cycles = readCycles();
        const auto start_time = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(kCalibrationDuration);

        const Cycles end_cycles = readCycles();
        const auto end_time = std::chrono::steady_clock::now();

        const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        const Cycles elapsed_cycles = end_cycles - start_cycles;

        cycles_per_ns_ = static_cast<double>(elapsed_cycles) / static_cast<double>(elapsed_ns);
    }

    double Clock::toNanos(Cycles cycles)
    {
        return static_cast<double>(cycles) / cycles_per_ns_;
    }
}