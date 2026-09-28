#pragma once

#include <cstdint>
#include <vector>
#include <limits>

namespace hft::bench
{
    class Histogram
    {
        public:
            Histogram();

            void record(std::uint64_t nanos);

            std::uint64_t min() const;
            std::uint64_t max() const;
            std::uint64_t count() const;
            std::uint64_t percentile(double p) const;

        private:
            std::vector<std::uint64_t> buckets_;
            std::uint64_t min_ = std::numeric_limits<std::uint64_t>::max();
            std::uint64_t max_ = 0;
            std::uint64_t count_ = 0;

            static std::size_t bucketIndex(std::uint64_t nanos);
    };
}