#include "bench/histogram.hpp"

#include <bit>
#include <cmath>

namespace hft::bench
{
    namespace
    {
        constexpr std::size_t kNumBuckets = 65;
    }

    Histogram::Histogram()
        : buckets_(kNumBuckets, 0)
    {
    }

    void Histogram::record(std::uint64_t nanos)
    {
        ++buckets_[bucketIndex(nanos)];
        ++count_;
        if(nanos > max_)
        {
            max_ = nanos;
        }
        if(nanos < min_)
        {
            min_ = nanos;
        }
    }

    std::uint64_t Histogram::percentile(double p) const
    {
        if(count_ == 0)
        {
            return 0;
        }

        const auto target_rank = static_cast<std::uint64_t>(std::ceil((p / 100.0) * static_cast<double>(count_)));

        std::uint64_t cumulative = 0;
        for(std::size_t i = 0; i < kNumBuckets; ++i)
        {
            cumulative += buckets_[i];
            if(cumulative >= target_rank)
            {
                return i== 0 ? 0 : (std::uint64_t(1) << (i - 1));
            }
        }

        return max_;
    }

    std::uint64_t Histogram::min() const
    {
        return count_ == 0 ? 0 : min_;
    }

    std::uint64_t Histogram::max() const
    {
        return max_;
    }

    std::uint64_t Histogram::count() const
    {
        return count_;
    }

    std::size_t Histogram::bucketIndex(std::uint64_t nanos)
    {
        if(nanos == 0)
        {
            return 0;
        }
        return static_cast<std::size_t>(64-std::countl_zero(nanos));
    }
}