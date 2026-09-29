#include "bench/histogram.hpp"

#include <gtest/gtest.h>

using namespace hft::bench;

TEST(HistogramTest, EmptyHistogramReturnsZero) {
    Histogram h;
    EXPECT_EQ(h.count(), 0u);
    EXPECT_EQ(h.min(), 0u);
    EXPECT_EQ(h.max(), 0u);
    EXPECT_EQ(h.percentile(50), 0u);
}

TEST(HistogramTest, TracksMinAndMax) {
    Histogram h;
    h.record(100);
    h.record(50);
    h.record(200);

    EXPECT_EQ(h.count(), 3u);
    EXPECT_EQ(h.min(), 50u);
    EXPECT_EQ(h.max(), 200u);
}

TEST(HistogramTest, PercentilesAreMonotonicallyNonDecreasing) {
    Histogram h;
    for (std::uint64_t v = 1; v <= 1000; ++v) {
        h.record(v);
    }

    EXPECT_LE(h.percentile(50), h.percentile(99));
    EXPECT_LE(h.percentile(99), h.percentile(99.9));
    EXPECT_LE(h.percentile(99.9), h.max());
}