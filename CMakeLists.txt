cmake_minimum_required(VERSION 3.16)
project(orderbook)
enable_testing()
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST REQUIRED gtest)

add_library(orderbook
    exchange_pq.cpp
    exchange_multiset.cpp
)

add_executable(orderbook_test exchange_test.cpp)
target_link_libraries(orderbook_test ${GTEST_LIBRARIES} orderbook)
target_link_directories(orderbook_test PRIVATE ${GTEST_LIBRARY_DIRS})
target_include_directories(orderbook_test PRIVATE ${GTEST_INCLUDE_DIRS})

