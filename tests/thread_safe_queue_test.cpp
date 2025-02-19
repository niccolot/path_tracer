#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <iostream>

#include "multithreading.h"

TEST_CASE("ThreadSafeQueue class tes") {
    ThreadSafeQueue<std::shared_ptr<int>> q;

    REQUIRE(1 == 1);
}
