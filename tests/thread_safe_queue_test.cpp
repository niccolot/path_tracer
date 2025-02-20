#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <iostream>

#include "multithreading.h"

typedef struct TestStruct {
        int n = 0;
        std::vector<double> vec;
} test_struct_t;

TEST_CASE("ThreadSafeQueue class test single thread") {

    ThreadSafeQueue<test_struct_t> q;
    test_struct_t s;
    test_struct_t s2;
    
    SECTION("empty(), push() test") {
        REQUIRE(q.empty() == true);

        q.push(s);
        REQUIRE(q.empty() == false);
    }

    SECTION("wait_and_pop() test") {
        q.push(s);
        auto res = q.wait_and_pop();
        REQUIRE(res);
        REQUIRE(q.empty() == true);

        s.n = 1;
        s.vec = std::vector<double>(10, 1.5);
        q.push(s);
        test_struct_t res2;
        q.wait_and_pop(res2);
        REQUIRE(q.empty() == true);
        REQUIRE(res2.n == 1);
        REQUIRE(res2.vec[0] == 1.5);
        REQUIRE(res2.vec.size() == 10);

        s2.n = 2;
        s2.vec = std::vector<double>(10, 2.5);
        q.push(s);
        q.push(s2);

        q.wait_and_pop(*res);
        q.wait_and_pop(res2);

        REQUIRE(res -> n == 1);
        REQUIRE(res2.n == 2);
        REQUIRE(res -> vec[0] == 1.5);
        REQUIRE(res2.vec[0] == 2.5);
    }

    SECTION("try_pop() test") {
        test_struct_t res;
        REQUIRE(q.try_pop(res) == false);
        REQUIRE(!q.try_pop());

        q.push(s);
        auto res2 = q.try_pop();
        REQUIRE(res2);

        s.n = 1;
        s.vec = std::vector<double>(10, 1.5);
        q.push(s);
        test_struct_t res3;

        q.try_pop(res3);
        REQUIRE(res3.n == 1);
        REQUIRE(res3.vec[0] == 1.5);
        REQUIRE(res3.vec.size() == 10);

        s2.n = 2;
        s2.vec = std::vector<double>(10, 2.5);

        q.push(s);
        q.push(s2);

        auto res4 = q.try_pop();
        test_struct_t res5;
        q.try_pop(res5);
        REQUIRE(res4 -> n == 1);
        REQUIRE(res5.n == 2);
        REQUIRE(res4 -> vec[0] == 1.5);
        REQUIRE(res5.vec[0] == 2.5);
    }   
}

TEST_CASE("Thread safe queue multithreaded") {
    
}