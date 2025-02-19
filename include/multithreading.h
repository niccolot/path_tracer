#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "vec3.h"

typedef struct JobBlock {
    int row_start = 0;
    int row_end = 0;
    int row_size;
    std::vector<int> indices;
    std::vector<Vec3> colors;
} job_block_t; // block_job_t 

class ThreadJoiner {
private:
    std::vector<std::thread>& m_threads;

public:
    explicit ThreadJoiner(std::vector<std::thread>& threads) : m_threads(threads) {}
    ~ThreadJoiner() {
        for (auto& t : m_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
}; // class ThreadJoiner

template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable cv;

public:
    ThreadSafeQueue() {}

    void wait_and_pop(T& val) {
        std::unique_lock<std::mutex> lk(mut);
        cv.wait(lk, [this] { return !data_queue.empty(); });
        val = std::move(*data_queue.front());
        data_queue.pop();
    }

    bool try_pop(T& val) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) {
            return false;
        }

        val = std::move(*data_queue.front());
        data_queue.pop();

        return true;
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lk(mut);
        cv.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();

        return res;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();

        return res;
    }

    void push(T val) {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(val)));
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        cv.notify_one();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);

        return data_queue.empty();
    }
}; // class ThreadSafeQueue 
#endif