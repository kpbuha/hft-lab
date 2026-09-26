#pragma once

#include <cstdint>

namespace hft::book
{

    // Price is stored as an integer number of ticks, 
    // e.g. with a $0.01 tick size, $152.25 is stored as 15225
    // These avoids floating point comparisons erros and also can be directly used as index
    using Price = std::int64_t;

    using Qty = std::uint32_t;

    // Unique identifier assigned to each order, used to identify order for cancellation
    using OrderId = std::uint64_t;

    // Strictly increasing number the book assigns to each order as it arrives.
    // It gives FIFO time-priority at same price level without reading the clock (which is slower and can collide).
    using SeqNum = std::uint64_t;

    enum class Side : std::uint8_t
    {
        Buy,
        Sell
    };
}