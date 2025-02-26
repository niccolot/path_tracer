#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <iostream>
#include <future>

#include "multithreading.h"

TEST_CASE("construction and destruction test") {
    ThreadPool pool;
}

