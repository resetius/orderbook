#include <gtest/gtest.h>
#include <random>
#include <algorithm>
#include <vector>
#include <chrono>

#include <algorithm>
#include <vector>
#include <list>

#include "exchange_pq.hpp"
#include "exchange_multiset.hpp"

struct BookEntry {
    OrderId id;
    Side side;
    Price price;
    Quantity size;
};

void random_orders(std::vector<BookEntry>& orders, int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> price_dist(100, 200);
    std::uniform_int_distribution<> size_dist(1, 100);
    std::uniform_int_distribution<> side_dist(0, 1);

    for (OrderId i = 0; i < count; ++i) {
        orders.push_back({
            i,
            side_dist(gen) == 0 ? Side::Bid : Side::Ask,
            price_dist(gen),
            size_dist(gen)
        });
    }
}

TEST(ExchangeTest, RandomOrderMatching) {
    std::vector<BookEntry> randomOrders;
    random_orders(randomOrders, 1000000);

    TradeReporter reporter1, reporter2;
    ExchangePq exchangePq(reporter1);
    ExchangeMultiset exchangeMultiset(reporter2);

    auto t0 = std::chrono::high_resolution_clock::now();

    for (const auto& order : randomOrders) {
        exchangePq.add(order.id, order.side, order.price, order.size);
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    for (const auto& order : randomOrders) {
        exchangeMultiset.add(order.id, order.side, order.price, order.size);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed2 = t1 - t0, elapsed1 = t2 - t1;
    std::cout << "Elapsed time: " << elapsed1.count() << " s " << elapsed2.count() << " s\n";

    auto exchangePqBids = exchangePq.ConsumeLower();
    auto exchangePqAsks = exchangePq.ConsumeUpper();

    auto exchangeMultisetBids = exchangeMultiset.ConsumeLower();
    auto exchangeMultisetAsks = exchangeMultiset.ConsumeUpper();

    ASSERT_EQ(exchangePqBids.size(), exchangeMultisetBids.size());
    ASSERT_EQ(exchangePqAsks.size(), exchangeMultisetAsks.size());

    size_t i = 0;
    for (; i < exchangePqBids.size(); ++i) {
        ASSERT_EQ(std::get<0>(exchangePqBids[i]), std::get<0>(exchangePqBids[i]));
        ASSERT_EQ(std::get<1>(exchangePqBids[i]), std::get<1>(exchangeMultisetBids[i]));
    }

    i = 0;
    for (; i < exchangePqAsks.size(); ++i) {
        ASSERT_EQ(std::get<0>(exchangePqAsks[i]), std::get<0>(exchangeMultisetAsks[i]));
        ASSERT_EQ(std::get<1>(exchangePqAsks[i]), std::get<1>(exchangeMultisetAsks[i]));
        i++;
    }

    ASSERT_EQ(reporter1.reported.size(), reporter2.reported.size());
    for (size_t i = 0; i < reporter1.reported.size(); ++i) {
        ASSERT_EQ(reporter1.reported[i], reporter2.reported[i]);
    }
}

void random_operations_with_delete(std::vector<std::pair<int, BookEntry>>& ops, int count, int delete_chance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> price_dist(100, 200);
    std::uniform_int_distribution<> size_dist(1, 100);
    std::uniform_int_distribution<> side_dist(0, 1);
    std::uniform_int_distribution<> op_type_dist(0, 99);
    std::uniform_int_distribution<> id_dist(0, count - 1);

    for (OrderId i = 0; i < count; ++i) {
        if (op_type_dist(gen) < delete_chance && !ops.empty()) {
            int delete_index = id_dist(gen) % ops.size();
            ops.push_back({0, ops[delete_index].second});
        } else {
            BookEntry entry = {
                i,
                side_dist(gen) == 0 ? Side::Bid : Side::Ask,
                price_dist(gen),
                size_dist(gen)
            };
            ops.push_back({1, entry});
        }
    }
}

TEST(ExchangeTest, RandomAddDeleteOperations) {
    std::vector<std::pair<int, BookEntry>> operations;
    random_operations_with_delete(operations, 100000, 25);

    TradeReporter reporter1;
    TradeReporter reporter2;
    ExchangeMultiset exchangeMultiset(reporter1);
    ExchangePq exchangePq(reporter2);

    auto t0 = std::chrono::high_resolution_clock::now();

    for (const auto& op : operations) {
        if (op.first == 1) {
            exchangeMultiset.add(op.second.id, op.second.side, op.second.price, op.second.size);
        } else if (op.first == 0) {
            exchangeMultiset.erase(op.second.id);
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    for (const auto& op : operations) {
        if (op.first == 1) {
            exchangePq.add(op.second.id, op.second.side, op.second.price, op.second.size);
        } else if (op.first == 0) {
            exchangePq.erase(op.second.id);
        }
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed1 = t1 - t0, elapsed2 = t2 - t1;
    std::cout << "Elapsed time: " << elapsed1.count() << " s " << elapsed2.count() << " s\n";

    ASSERT_EQ(reporter1.reported.size(), reporter2.reported.size());
    for (size_t i = 0; i < reporter1.reported.size(); ++i) {
        ASSERT_EQ(reporter1.reported[i], reporter2.reported[i]);
    }
}

TEST(ExchangeTest, ConsumeAllAsksByOneBigBidPq) {
    TradeReporter reporter;
    ExchangePq exchange(reporter);

    auto t0 = std::chrono::high_resolution_clock::now();

    Price initialPrice = 100;
    Quantity askSize = 10;
    for (int i = 0; i < 10000000; ++i) {
        exchange.add(i, Side::Ask, initialPrice + i, askSize); 
    }

    OrderId bigBidId = 10000000;
    Price bigBidPrice = initialPrice + 9999999; 
    Quantity bigBidSize = 100000000; 
    exchange.add(bigBidId, Side::Bid, bigBidPrice, bigBidSize);

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = t1 - t0;
    std::cout << "Elapsed time: " << elapsed1.count() << " s\n";

    auto reportedTrades = reporter.reported;
    ASSERT_EQ(reportedTrades.size(), 20000000); 

    Quantity totalMatched = 0;
    for (const auto& trade : reportedTrades) {
        totalMatched += std::get<2>(trade); 
    }
    ASSERT_EQ(totalMatched, 200000000); 
}

TEST(ExchangeTest, ConsumeAllAsksByOneBigBidMultiset) {
    TradeReporter reporter;
    ExchangeMultiset exchange(reporter);

    auto t0 = std::chrono::high_resolution_clock::now();

    Price initialPrice = 100;
    Quantity askSize = 10;
    for (int i = 0; i < 10000000; ++i) {
        exchange.add(i, Side::Ask, initialPrice + i, askSize); 
    }

    OrderId bigBidId = 10000000;
    Price bigBidPrice = initialPrice + 9999999; 
    Quantity bigBidSize = 100000000; 
    exchange.add(bigBidId, Side::Bid, bigBidPrice, bigBidSize);

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = t1 - t0;
    std::cout << "Elapsed time: " << elapsed1.count() << " s\n";

    auto reportedTrades = reporter.reported;
    ASSERT_EQ(reportedTrades.size(), 20000000); 

    Quantity totalMatched = 0;
    for (const auto& trade : reportedTrades) {
        totalMatched += std::get<2>(trade);
    }
    ASSERT_EQ(totalMatched, 200000000); 
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

