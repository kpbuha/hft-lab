#pragma once

#include "types.hpp"

namespace hft::book
{
    
struct Order
{
    OrderId id{};
    Side side{};
    Price price{};
    Qty qty{};
    SeqNum seq{};
};

}