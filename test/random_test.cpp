#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <iostream>

#include "random.h"

TEST_CASE("Random functions") {
uint32_t samples = 100000;

BENCHMARK("random_float(), xorshift") {
    float x;
    for(uint32_t i = 0; i < samples; ++i) {
        x  = RandomUtils::random_float();
    }
};
BENCHMARK("random_float(), mt19997") {
    float x;
    for(uint32_t i = 0; i < samples; ++i) {
        x  = RandomUtils::random_float(true);
    }
};
BENCHMARK("random_float(min, max), xorshift") {
    float x;
    for(uint32_t i = 0; i < samples; ++i) {
        x  = RandomUtils::random_float(i,i+1);
    }
};
BENCHMARK("random_float(min, max), mt19997") {
    float x;
    for(uint32_t i = 0; i < samples; ++i) {
        x  = RandomUtils::random_float(i,i+1,true);
    }
};
BENCHMARK("random_int(min, max), xorshift") {
    uint32_t x;
    for(uint32_t i = 0; i < samples; ++i) {
        x  = RandomUtils::random_int(i,i + 10);
    }
};
BENCHMARK("random_float(min, max), mt19997") {
    float x;
    for(uint32_t i = 0; i < samples; ++i) {
        x  = RandomUtils::random_int(i,i + 10,true);
    }
};
}