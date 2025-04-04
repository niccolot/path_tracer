#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <iostream>
#include <future>

#include "multithreading.h"

typedef struct TestStruct {
        int n = 0;
        std::vector<double> vec{};
} test_struct_t;

TEST_CASE("ThreadSafeQueue class test single thread") {

    ThreadSafeQueue<test_struct_t> q;
    test_struct_t s;
    test_struct_t s2;
    test_struct_t s3;
    
    SECTION("empty(), push() test") {
        REQUIRE(q.empty());

        q.push(std::move(s));
        REQUIRE(!q.empty());
    }

    SECTION("wait_and_pop() test") {
        q.push(std::move(s));
        auto res = q.wait_and_pop();
        REQUIRE(q.empty());

        s.n = 1;
        s.vec = std::move(std::vector<double>(10, 1.5));
        q.push(std::move(s));
        test_struct_t res2;
        q.wait_and_pop(res2);
        REQUIRE(q.empty());
        REQUIRE(res2.n == 1);
        REQUIRE(res2.vec[0] == 1.5);
        REQUIRE(res2.vec.size() == 10);

        s2.n = 2;
        s2.vec = std::move(std::vector<double>(10, 2.5));
        s3.n = 3;
        s3.vec = std::move(std::vector<double>(10, 3.5));
        q.push(std::move(s2));
        q.push(std::move(s3));

        q.wait_and_pop(res);
        q.wait_and_pop(res2);

        REQUIRE(res.n == 2);
        REQUIRE(res2.n == 3);
        REQUIRE(res.vec[0] == 2.5);
        REQUIRE(res.vec.size() == 10);
        REQUIRE(res2.vec.size() == 10);
        REQUIRE(res2.vec[0] == 3.5);
    }

    SECTION("try_pop() test") {
        test_struct_t res;
        REQUIRE(!q.try_pop(res));
        REQUIRE(!q.try_pop());

        q.push(std::move(s));
        auto res2 = q.try_pop();
        REQUIRE(res2);

        s.n = 1;
        s.vec = std::vector<double>(10, 1.5);
        q.push(std::move(s));
        test_struct_t res3;

        q.try_pop(res3);
        REQUIRE(res3.n == 1);
        REQUIRE(res3.vec[0] == 1.5);
        REQUIRE(res3.vec.size() == 10);

        s2.n = 2;
        s2.vec = std::move(std::vector<double>(10, 2.5));
        s3.n = 3;
        s3.vec = std::move(std::vector<double>(10, 3.5));

        q.push(std::move(s2));
        q.push(std::move(s3));

        auto res4 = q.try_pop();
        test_struct_t res5;
        q.try_pop(res5);
        REQUIRE(res4 -> n == 2);
        REQUIRE(res5.n == 3);
        REQUIRE(res4 -> vec[0] == 2.5);
        REQUIRE(res5.vec[0] == 3.5);
    }   
}

TEST_CASE("Thread safe queue multithreaded") {

    ThreadSafeQueue<test_struct_t> q;
    std::vector<std::thread> threads;
    test_struct_t s {1, std::vector<double>(5, 1.5)};
    test_struct_t s2 {2, std::vector<double>(5, 2.5)};
    test_struct_t s3 {3, std::vector<double>(5, 3.5)};

    SECTION("empty(), push() test") {
        threads.emplace_back(std::thread([&] {
            q.push(std::move(s));
        }));
        threads.emplace_back(std::thread([&] {
            q.push(std::move(s2));
        }));
        threads.emplace_back(std::thread([&] {
            q.push(std::move(s3));
        }));

        for (auto& t : threads) {
            t.join();
        }

        REQUIRE(!q.empty());
    }

    SECTION("wait_and_pop() test") {
        test_struct_t res, res2, res3;
        threads.emplace_back(std::thread([&] {
            q.wait_and_pop(res);
        }));
        threads.emplace_back(std::thread([&] {
            q.wait_and_pop(res2);
        }));
        threads.emplace_back(std::thread([&] {
            q.wait_and_pop(res3);
        }));

        threads.emplace_back(std::thread([&] {
            q.push(std::move(s));
        }));
        threads.emplace_back(std::thread([&] {
            q.push(std::move(s2));
        }));
        threads.emplace_back(std::thread([&] {
            q.push(std::move(s3));
        }));

        for (auto& t : threads) {
            t.join();
        }

        REQUIRE(res.vec.size() == 5);
        REQUIRE(res2.vec.size() == 5);
        REQUIRE(res3.vec.size() == 5);
    }

    SECTION("try_pop() test") {
        test_struct_t res, res2, res3;
        
        threads.emplace_back(std::thread([&] {
            q.try_pop(res);
        }));
        threads.emplace_back(std::thread([&] {
            q.try_pop(res2);
        }));
        threads.emplace_back(std::thread([&] {
            q.try_pop(res3);
        }));

        for (auto& t : threads) {
            t.join();
        }

        REQUIRE(res.vec.size() == 0);
        REQUIRE(res2.vec.size() == 0);
        REQUIRE(res3.vec.size() == 0);

        auto num_threads = std::thread::hardware_concurrency();
        std::vector<std::future<std::optional<test_struct_t>>> futures;

        for (unsigned i = 0; i < num_threads; ++i) {
            futures.emplace_back(std::async(std::launch::async, 
            [&]() -> std::optional<test_struct_t> {
                return q.try_pop();
            }));
        }

        REQUIRE(futures.size() == num_threads);

        for (auto& f : futures) {
            auto _ = f.get();
        }
    }
}
