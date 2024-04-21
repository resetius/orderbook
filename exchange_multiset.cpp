#include "exchange_multiset.hpp"


void ExchangeMultiset::add(OrderId id, Side side, Price price, Quantity quantity) {
    BookEntry entry = {id, side, price, quantity, currentTime++};

    if (side == Side::Bid) {
        auto it = asks.begin();
        while (it != asks.end() && entry.size > 0) {
            if (entry.price >= it->price) {
                Quantity tradeSize = std::min(entry.size, it->size);
                entry.size -= tradeSize;
                BookEntry updated_ask = *it;
                updated_ask.size -= tradeSize;

                reporter.on_trade(entry.id, it->price, tradeSize);
                reporter.on_trade(it->id, it->price, tradeSize);

                asks.erase(it);
                if (updated_ask.size > 0) {
                    asks.insert(updated_ask);
                }
                if (entry.size == 0) break;
            } else {
                break;
            }
            it = asks.begin();
        }
        if (entry.size > 0) {
            bids.insert(entry);
        }
    } else { // Side::ASK
        auto it = bids.begin();
        while (it != bids.end() && entry.size > 0) {
            if (it->price >= entry.price) {
                Quantity tradeSize = std::min(entry.size, it->size);
                entry.size -= tradeSize;
                BookEntry updated_bid = *it;
                updated_bid.size -= tradeSize;

                reporter.on_trade(it->id, it->price, tradeSize);
                reporter.on_trade(entry.id, it->price, tradeSize);

                bids.erase(it);
                if (updated_bid.size > 0) {
                    bids.insert(updated_bid);
                }
                if (entry.size == 0) break;
            } else {
                break;
            }
            it = bids.begin();
        }
        if (entry.size > 0) {
            asks.insert(entry);
        }
    }
}

void ExchangeMultiset::erase(OrderId id) {
    auto remove_func = [id](const BookEntry& entry) { return entry.id == id; };
    auto bid_it = std::find_if(bids.begin(), bids.end(), remove_func);
    if (bid_it != bids.end()) bids.erase(bid_it);

    auto ask_it = std::find_if(asks.begin(), asks.end(), remove_func);
    if (ask_it != asks.end()) asks.erase(ask_it);
}

