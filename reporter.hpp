#pragma once

struct TradeReporter {
    std::vector<std::tuple<OrderId,Price,i64>> reported;

    void on_trade(OrderId id, Price price, i64 size) {
        reported.emplace_back(id, price, size);
    }
};

