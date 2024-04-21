# Orderbook Implementations

This project includes two implementations of an orderbook: one using `priority_queue` and another using `multiset`. The corresponding classes are `ExchangePq` for the `priority_queue` implementation, and `ExchangeMultiset` for the `multiset` implementation.

## Features

- **ExchangePq**: Implements the orderbook using `std::priority_queue`. This implementation is optimized for operations that frequently access the top element but might be slower for mid-container operations.
  
- **ExchangeMultiset**: Utilizes `std::multiset` to manage the orderbook, providing efficient handling and flexibility for insertion and deletion across the entire data structure.

## Usage

See `exchange_test.cpp` for examples and test cases illustrating how to use both implementations.

### Adding Orders

To add a bid or ask order to the orderbook, use the `add` method:

```cpp
exchangePq.add(orderId, orderSide, orderPrice, orderQuantity);
exchangeMultiset.add(orderId, orderSide, orderPrice, orderQuantity);
```

### Removing Orders

To remove an order from the orderbook, use the `erase` method:

```cpp
exchangePq.erase(orderId);
exchangeMultiset.erase(orderId);
```

## Building

The project is set up to use `cmake` for building. Follow these steps to compile:

1. Create a build directory:

   ```bash
   mkdir build
   cd build
   ```

2. Run CMake and make:

   ```bash
   cmake ..
   make
   ```

3. Run the tests (optional):

   ```bash
   ./exchange_test
   ```

For more information on how to set up and run the tests, refer to the documentation in `exchange_test.cpp`.
