#include "exchange_pq.hpp"

ExchangePq::ExchangePq(TradeReporter &trade_reporter)
    : reporter(trade_reporter)
    , internalId(1)
{
}

void ExchangePq::report(BookEntry& order, BookEntry& entry)
{
    auto size = std::min(order.size, entry.size);
    // bid, ask
    if (order.side == Side::Bid) {
        reporter.on_trade(order.id, entry.price, size);
        reporter.on_trade(entry.id, entry.price, size);
    } else {
        reporter.on_trade(entry.id, entry.price, size);
        reporter.on_trade(order.id, entry.price, size);      
    }  
}

bool ExchangePq::processEntry(BookEntry& order, BookEntry& entry)
{
    if (entry.size <= order.size) {
        report(order, entry);
        order.size -= entry.size;
        book.erase(entry.id);
        return true;
    } else { // entry.size > order.size
        report(order, entry);
        entry.size -= order.size;
        order.size = 0;
        return false;
    }
}

template<typename Q>
void ExchangePq::processQueue(Q& queue, BookEntry& order, int direction) {
    while (order.size && !queue.empty()) {
        auto [price, internalId, id] = queue.top();
        if (direction * (price - order.price) > 0) {
            break;
        }
        auto it = book.find(id);
        if (it == book.end()) {
          // deleted entry
          queue.pop();
          continue;
        }
        if (processEntry(order, it->second)) {
          queue.pop();
        }
    }
}

void ExchangePq::add(OrderId id, Side side, Price price, Quantity quantity)
{
    BookEntry order {
        .id = id,
        .side = side,
        .price = price,
        .size = quantity
    };

    if (side == Side::Bid) { // buyer
        processQueue(upper, order, 1);
    } else { // side == Side::Ask // seller
        processQueue(lower, order, -1);
    }

    if (order.size) {
        book[id] = order;
        if (side == Side::Bid) {
            lower.emplace(price, -internalId, id);
        } else {
            upper.emplace(price, internalId, id);
        }
        internalId++;
    }
}

void ExchangePq::erase(OrderId id) {
    book.erase(id);
}

std::vector<std::tuple<Price,OrderId>> ExchangePq::ConsumeLower() {
    std::vector<std::tuple<Price,OrderId>> ret;
    while (!lower.empty()) {
        auto [price, _, id] = lower.top();
        ret.emplace_back(price, id); lower.pop();
    }
    return ret;
}

std::vector<std::tuple<Price,OrderId>> ExchangePq::ConsumeUpper() {
    std::vector<std::tuple<Price,OrderId>> ret;
    while (!upper.empty()) {
        auto [price, _, id] = upper.top();
        ret.emplace_back(price, id); upper.pop();
    }
    return ret;
}

