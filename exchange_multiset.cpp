#include "exchange_multiset.hpp"

void ExchangeMultiset::process(BookEntry& entry, std::multiset<BookEntry>& match, std::multiset<BookEntry>& insert, int direction)
{
    auto it = match.begin();
    while (it != match.end() && entry.size > 0 && (direction * (entry.price - it->price) >= 0)) {
        Quantity tradeSize = std::min(entry.size, it->size);
        entry.size -= tradeSize;
        BookEntry updated_match = *it;
        updated_match.size -= tradeSize;

        if (direction == 1) {
            reporter.on_trade(entry.id, it->price, tradeSize);
            reporter.on_trade(it->id, it->price, tradeSize);
        } else {
            reporter.on_trade(it->id, it->price, tradeSize);
            reporter.on_trade(entry.id, it->price, tradeSize);
        }

        match.erase(it);
        if (updated_match.size > 0) {
            match.insert(updated_match);
        }
        it = match.begin();
    }
    if (entry.size > 0) {
        insert.insert(entry);
    }
}

void ExchangeMultiset::add(OrderId id, Side side, Price price, Quantity quantity) {
    BookEntry entry = {id, side, price, quantity, currentTime++};

    if (side == Side::Bid) {
        process(entry, asks, bids, 1);
    } else { // Side::Ask
        process(entry, bids, asks, -1);
    }   
}

void ExchangeMultiset::erase(OrderId id) {
    auto remove_func = [id](const BookEntry& entry) { return entry.id == id; };
    auto bid_it = std::find_if(bids.begin(), bids.end(), remove_func);
    if (bid_it != bids.end()) bids.erase(bid_it);

    auto ask_it = std::find_if(asks.begin(), asks.end(), remove_func);
    if (ask_it != asks.end()) asks.erase(ask_it);
}

