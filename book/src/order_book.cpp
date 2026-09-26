#include "book/order_book.hpp"
#include "book/trade.hpp"

using namespace hft::book;

AddResult OrderBook::add(Order order)
{
    order.seq = next_seq_++;
    AddResult result{{}, order.qty};
    // Implementation for adding order to the book  
    if(order.side == Side::Buy)
    {
        while(!asks_.empty() && result.remaining_qty > 0 && order.price >= asks_.begin()->first)
        {
            auto it = asks_.begin();
            Trade trade{order.id, it->second.front().id,it->second.front().price,0};
            if(it->second.front().qty > result.remaining_qty)
            {
                trade.qty = result.remaining_qty;
                it->second.front().qty -= result.remaining_qty;
                result.remaining_qty = 0;
            }
            else
            {
                trade.qty = it->second.front().qty;
                result.remaining_qty -= it->second.front().qty;
                it->second.pop_front();
                locations_.erase(trade.resting_id);
                if (it->second.empty()) { asks_.erase(it); }
            }
            result.trades.push_back(trade);
        }

        if(result.remaining_qty > 0)
        {
            order.qty = result.remaining_qty;
            bids_[order.price].push_back(order);
            locations_[order.id] = {Side::Buy, order.price, std::prev(bids_[order.price].end())};
        }
    }
    else
    {
        while(!bids_.empty() && result.remaining_qty > 0 && order.price <= bids_.begin()->first)
        {
            auto it = bids_.begin();
            Trade trade{order.id, it->second.front().id,it->second.front().price,0};
            if(it->second.front().qty > result.remaining_qty)
            {
                trade.qty = result.remaining_qty;
                it->second.front().qty -= result.remaining_qty;
                result.remaining_qty = 0;
            }
            else
            {
                trade.qty = it->second.front().qty;
                result.remaining_qty -= it->second.front().qty;
                it->second.pop_front();
                locations_.erase(trade.resting_id);
                if (it->second.empty()) { bids_.erase(it); }
            }
            result.trades.push_back(trade);
        }

        if(result.remaining_qty > 0)
        {
            order.qty = result.remaining_qty;
            asks_[order.price].push_back(order);
            locations_[order.id] = {Side::Sell, order.price, std::prev(asks_[order.price].end())};
        }
    }

    return result;
}

std::optional<Price> OrderBook::bestBid() const
{
    if(bids_.empty())
        return std::nullopt;

    return bids_.begin()->first;
}

std::optional<Price> OrderBook::bestAsk() const
{
    if(asks_.empty())
        return std::nullopt;

    return asks_.begin()->first;
}

bool OrderBook::cancel(OrderId id)
{
    auto it = locations_.find(id);
    if(it != locations_.end())
    {
        auto& loc = it->second;
        if(loc.side == Side::Buy)
        {
            bids_[loc.price].erase(loc.it);
            if(bids_[loc.price].empty())
                bids_.erase(loc.price);
            locations_.erase(it);
            return true;
        }
        else
        {
            asks_[loc.price].erase(loc.it);
            if(asks_[loc.price].empty())
                asks_.erase(loc.price);
            locations_.erase(it);
            return true;
        }
    }
    return false;
}

Qty OrderBook::quantityAt(Side side, Price price) const
{
    if(side == Side::Buy)
    {
        if(bids_.find(price) != bids_.end())
        {
            Qty total_qty = 0;
            for(const auto& order : bids_.at(price))
            {
                total_qty += order.qty;
            }
            return total_qty;
        }
    }
    else
    {
        if(asks_.find(price) != asks_.end())
        {
            Qty total_qty = 0;
            for(const auto& order : asks_.at(price))
            {
                total_qty += order.qty;
            }
            return total_qty;
        }
    }
    return 0;
}