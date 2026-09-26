#include "book/order_book.hpp"

#include <gtest/gtest.h>

using namespace hft::book;

TEST(OrderBookTest, AddRestingOrder) {
    OrderBook book;

    Order buy{1, Side::Buy, 100, 10, 0};
    AddResult result = book.add(buy);

    EXPECT_TRUE(result.trades.empty());
    EXPECT_EQ(result.remaining_qty, 10);

    ASSERT_TRUE(book.bestBid().has_value());
    EXPECT_EQ(*book.bestBid(), 100);
    EXPECT_FALSE(book.bestAsk().has_value());
}

TEST(OrderBookTest, AddFullyCrossingOrder) {
    OrderBook book;

    book.add(Order{1, Side::Sell, 100, 10, 0});
    AddResult result = book.add(Order{2, Side::Buy, 100, 10, 0});

    ASSERT_EQ(result.trades.size(), 1u);
    EXPECT_EQ(result.trades[0].aggressor_id, 2u);
    EXPECT_EQ(result.trades[0].resting_id, 1u);
    EXPECT_EQ(result.trades[0].price, 100);
    EXPECT_EQ(result.trades[0].qty, 10u);
    EXPECT_EQ(result.remaining_qty, 0u);

    EXPECT_FALSE(book.bestAsk().has_value());
    EXPECT_FALSE(book.bestBid().has_value());
}

TEST(OrderBookTest, AddPartiallyFillingOrder) {
    OrderBook book;

    book.add(Order{1, Side::Sell, 100, 100, 0});
    AddResult result = book.add(Order{2, Side::Buy, 100, 60, 0});

    ASSERT_EQ(result.trades.size(), 1u);
    EXPECT_EQ(result.trades[0].qty, 60u);
    EXPECT_EQ(result.remaining_qty, 0u);

    EXPECT_EQ(book.quantityAt(Side::Sell, 100), 40u);
}

TEST(OrderBookTest, AddSweepingMultipleLevels) {
    OrderBook book;

    book.add(Order{1, Side::Sell, 100, 10, 0});
    book.add(Order{2, Side::Sell, 101, 10, 0});
    AddResult result = book.add(Order{3, Side::Buy, 101, 15, 0});

    ASSERT_EQ(result.trades.size(), 2u);
    EXPECT_EQ(result.trades[0].resting_id, 1u);
    EXPECT_EQ(result.trades[0].price, 100);
    EXPECT_EQ(result.trades[0].qty, 10u);

    EXPECT_EQ(result.trades[1].resting_id, 2u);
    EXPECT_EQ(result.trades[1].price, 101);
    EXPECT_EQ(result.trades[1].qty, 5u);

    EXPECT_EQ(result.remaining_qty, 0u);
    EXPECT_EQ(book.quantityAt(Side::Sell, 101), 5u);
}

TEST(OrderBookTest, CancelRestingOrder) {
    OrderBook book;

    book.add(Order{1, Side::Buy, 100, 10, 0});
    EXPECT_TRUE(book.cancel(1));
    EXPECT_FALSE(book.bestBid().has_value());
}

TEST(OrderBookTest, CancelUnknownOrderReturnsFalse) {
    OrderBook book;
    EXPECT_FALSE(book.cancel(999));
}

TEST(OrderBookTest, TimePriorityAtSamePrice) {
    OrderBook book;

    book.add(Order{1, Side::Sell, 100, 10, 0});  // arrives first
    book.add(Order{2, Side::Sell, 100, 10, 0});  // arrives second

    AddResult result = book.add(Order{3, Side::Buy, 100, 10, 0});

    ASSERT_EQ(result.trades.size(), 1u);
    EXPECT_EQ(result.trades[0].resting_id, 1u);  // the FIRST one, not the second

    EXPECT_EQ(book.quantityAt(Side::Sell, 100), 10u);  // id=2 untouched
}