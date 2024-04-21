#pragma once

#include <unordered_map>
#include <queue>
#include <list>
#include <cstdint>
#include <iostream>

#include "types.hpp"
#include "reporter.hpp"

class ExchangePq {
    using InternalId = u64;

    struct BookEntry {
        OrderId id;
        Side side;
        Price price;
        Quantity size;
    };

    std::priority_queue<std::tuple<Price,InternalId,OrderId>> lower;
    std::priority_queue<std::tuple<Price,InternalId,OrderId>,
        std::vector<std::tuple<Price,InternalId,OrderId>>,
        std::greater<std::tuple<Price,InternalId,OrderId>>  
        > upper;
    std::unordered_map<OrderId, BookEntry> book;
  
    TradeReporter& reporter;
    InternalId internalId;
    
    void report(BookEntry& order, BookEntry& entry);
    bool processEntry(BookEntry& order, BookEntry& entry);
  
    template<typename Q>
    void processQueue(Q& queue, BookEntry& order, int direction);
  
public:
    explicit ExchangePq(TradeReporter& reporter);
    void add(OrderId id, Side side, Price price, Quantity quantity);
    void erase(OrderId id);

    // TESTS
    std::vector<std::tuple<Price,OrderId>> ConsumeLower();
    std::vector<std::tuple<Price,OrderId>> ConsumeUpper();
};

