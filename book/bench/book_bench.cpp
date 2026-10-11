#include "book/order_book.hpp"
#include "bench/timer.hpp"
#include "bench/histogram.hpp"

#include <cstdio>
#include <random>
#include <vector>

using namespace hft::book;
using namespace hft::bench;

namespace
{
    constexpr int kWarmupOps = 5000;
    constexpr int kMeasuredOps = 1000000;
    constexpr Price kBasePrice = 100;
    constexpr Price kPriceSpread = 50;

    struct Workload
    {
        std::mt19937 rng{42};
        std::uniform_int_distribution<int> action_dist{0, 99};
        std::uniform_int_distribution<Price> price_offset_dist{-kPriceSpread, kPriceSpread};
        std::uniform_int_distribution<Qty> qty_dist{1, 100};
        OrderId next_id{1};
        std::vector<OrderId> live_orders;

        Order randomOrder(Side side) {
        Order o;
        o.id = next_id++;
        o.side = side;
        o.price = kBasePrice + price_offset_dist(rng);
        o.qty = qty_dist(rng);
        return o;
        }
    };

    void runOps(OrderBook& book, Workload& workload, int num_ops,
            Histogram* add_hist, Histogram* cancel_hist) 
    {
        for (int i = 0; i < num_ops; ++i) 
        {
            const int roll = workload.action_dist(workload.rng);

            if (roll < 90 && !workload.live_orders.empty()) 
            {
                std::uniform_int_distribution<std::size_t> pick(0, workload.live_orders.size() - 1);
                const std::size_t idx = pick(workload.rng);
                const OrderId id = workload.live_orders[idx];

                workload.live_orders[idx] = workload.live_orders.back();
                workload.live_orders.pop_back();

                const Cycles start = readCycles();
                book.cancel(id);
                const Cycles end = readCycles();

                if (cancel_hist) 
                {
                    cancel_hist->record(static_cast<std::uint64_t>(Clock::toNanos(end - start)));
                }
        } 
        else 
        {
            const Side side = (workload.action_dist(workload.rng) < 50) ? Side::Buy : Side::Sell;
            Order order = workload.randomOrder(side);

            const Cycles start = readCycles();
            AddResult result = book.add(order);
            const Cycles end = readCycles();

            if (add_hist) 
            {
                add_hist->record(static_cast<std::uint64_t>(Clock::toNanos(end - start)));
            }

            if (result.remaining_qty > 0) 
            {
                workload.live_orders.push_back(order.id);
            }
        }
    }
}

void printLine(const char* label, const Histogram& h) 
{
    std::printf("%-10s p50=%6llu ns   p99=%6llu ns   p99.9=%6llu ns   max=%6llu ns   n=%llu\n",
                label,
                static_cast<unsigned long long>(h.percentile(50)),
                static_cast<unsigned long long>(h.percentile(99)),
                static_cast<unsigned long long>(h.percentile(99.9)),
                static_cast<unsigned long long>(h.max()),
                static_cast<unsigned long long>(h.count()));
}

} // namespace

int main() {
    Clock::calibrate();

    OrderBook book;
    Workload workload;

    runOps(book, workload, kWarmupOps, nullptr, nullptr);

    Histogram add_latency;
    Histogram cancel_latency;

    runOps(book, workload, kMeasuredOps, &add_latency, &cancel_latency);

    printLine("add()", add_latency);
    printLine("cancel()", cancel_latency);

    return 0;
}