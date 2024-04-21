cmake_minimum_required(VERSION 3.16)
project(orderbook)
enable_testing()
set(CMAKE_CXX_STANDARD 20)

add_library(orderbook
    exchange_pq.cpp
    exchange_multiset.cpp
)

add_executable(orderbook_test exchange_test.cpp)
target_link_libraries(orderbook_test gtest orderbook)

