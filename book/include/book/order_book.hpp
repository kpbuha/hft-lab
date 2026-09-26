#pragma once

#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

#include "order.hpp"
#include "trade.hpp"
#include "types.hpp"


namespace hft::book
{

    // This is what Add() will handover to caller, trades the incoming order caused
    // and remaining qunatity over the resting in the book.
    struct AddResult
    {
        std::vector<Trade> trades;
        Qty remaining_qty{};
    };

    class OrderBook
{

    public:
        AddResult add(Order order);
        bool cancel(OrderId id);

        std::optional<Price> bestBid() const;
        std::optional<Price> bestAsk() const;
        Qty quantityAt(Side side, Price price) const;

    private:
        using Level = std::list<Order>;

        std::map<Price, Level, std::greater<Price>> bids_;
        std::map<Price, Level> asks_;

        struct OrderLocation
        {
            Side side{};
            Price price{};
            Level::iterator it;
        };

        std::unordered_map<OrderId, OrderLocation> locations_;

        SeqNum next_seq_{0};
};

}