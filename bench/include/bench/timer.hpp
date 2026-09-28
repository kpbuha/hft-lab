#pragma once

#include <cstdint>

namespace hft::bench
{
    using Cycles = std::uint64_t;

    Cycles readCycles();

    class Clock
    {
        public:
            static void calibrate();

            static double toNanos(Cycles cycles);

        private:
            static inline double cycles_per_ns_= 0.0;
    };
}