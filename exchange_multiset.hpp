#pragma once

#include <algorithm>
#include <vector>
#include <list>
#include <set>

#include "types.hpp"
#include "reporter.hpp"

class ExchangeMultiset {
private:
    struct BookEntry {
        OrderId id;
        Side side;
        Price price;
        Quantity size;
        u64 timestamp;

        bool operator<(const BookEntry& other) const {
            return (side == Side::Bid)
                ? (price > other.price || (price == other.price && timestamp < other.timestamp))
                : (price < other.price || (price == other.price && timestamp < other.timestamp));
        }
    };

    u64 currentTime = 0;
    std::multiset<BookEntry> bids;
    std::multiset<BookEntry> asks;
    TradeReporter& reporter;

public:
    ExchangeMultiset(TradeReporter& reporter) 
        : reporter(reporter) 
    { }

    void add(OrderId id, Side side, Price price, Quantity quantity);

    void erase(OrderId id);

    // TESTS
    std::vector<std::tuple<Price,OrderId>> ConsumeLower() {
        std::vector<std::tuple<Price,OrderId>> ret; ret.reserve(bids.size());
        for (auto& b : bids) {
            ret.emplace_back(b.price, b.id);
        }
        return ret;
    }

    std::vector<std::tuple<Price,OrderId>> ConsumeUpper() {
        std::vector<std::tuple<Price,OrderId>> ret; ret.reserve(asks.size());
        for (auto& b : asks) {
            ret.emplace_back(b.price, b.id);
        }
        return ret;
    }
};

