#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <iostream>
#include <future>
#include <stdio.h>

#include "multithreading.h"

TEST_CASE("construction and destruction test") {
    ThreadPool pool;
}

template<typename Iterator, typename T>
struct accumulate_block {
    T operator() (Iterator first, Iterator last) {
        return std::accumulate(first, last, T());
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned const length = std::distance(first, last);
    if (!length) {
        return init;
    }

    ThreadPool pool;
    unsigned const block_size = 20000;
    unsigned const num_blocks = (length + block_size - 1) / block_size;
    std::vector<std::future<T>> futures(num_blocks - 1);

    Iterator block_start = first;
    for (unsigned i = 0; i < (num_blocks - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submit([block_start, block_end]() {
            return accumulate_block<Iterator, T>()(block_start, block_end);
        });
        block_start = block_end;
    }

    T last_result = accumulate_block<Iterator, T>() (block_start, last);
    T result = init;
    for (unsigned i = 0; i < (num_blocks - 1); ++i) {
        result += futures[i].get();
    }

    result += last_result;

    return result;
}

TEST_CASE("parallel accumulate test") {
    unsigned const size = 1000000000;
    char message_serial[25];
    char message_parallel[25];
    sprintf(message_serial, "%u elements serial", size);
    sprintf(message_parallel, "%u elements parallel", size);
    std::vector<unsigned> vec(size);

    for (unsigned i = 0; i < size; ++i) {
        vec[i] = i;
    }
    
    BENCHMARK(message_serial) {
        return std::accumulate(vec.begin(), vec.end(), 0);
    };
    
    BENCHMARK(message_parallel) {
        return parallel_accumulate<std::vector<unsigned>::iterator, unsigned>(vec.begin(), vec.end(), 0);
    };
}
    

