#pragma once

#include "types.hpp"

namespace hft::book
{

    struct Trade
    {
        OrderId aggressor_id{};
        OrderId resting_id{};
        Price price{};
        Qty qty{};
    };

}