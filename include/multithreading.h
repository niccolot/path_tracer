#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include "vec3.h"

typedef struct JobBlock {
    int row_start;
    int row_end;
    int row_size;
    unsigned id;
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

class FunctionWrapper {
private:
    typedef struct ImplBase {
        virtual void call() = 0;
        virtual ~ImplBase() {}
    } impl_base_t; // impl_base_t

    template<typename F>
    struct impl_type : impl_base_t {
        F f;
        impl_type(F&& f_) : f(std::move(f_)) {}
        void call() { f(); }
    }; // struct impl_type

    std::unique_ptr<impl_base_t> impl;

public:
    template<typename F>
    FunctionWrapper(F&& f) : impl(new impl_type<F>(std::move(f))) {}
    FunctionWrapper() = default;
    FunctionWrapper(FunctionWrapper&& other) : impl(std::move(other.impl)) {}
    FunctionWrapper& operator=(FunctionWrapper&& other) {
        impl = std::move(other.impl);
        
        return *this;
    }

    void operator() () { impl->call(); }
    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
}; // class FunctionWrapper

class ThreadPool {
private:
    std::atomic<bool> done; 
    ThreadSafeQueue<FunctionWrapper> queue;
    std::vector<std::thread> threads;
    ThreadJoiner joiner;

    void worker_thread() {
        while(!done) {
            FunctionWrapper task;
            if (queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    ThreadPool() : done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                threads.push_back(std::thread(&ThreadPool::worker_thread, this));
            }
        } catch(...) {
            done = true;
            throw;
        }
    }

    ~ThreadPool() { done = true; }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task{std::move(f)};
        std::future<result_type> res(task.get_future());
        queue.push(std::move(task));

        return res;
    }
}; // class ThreadPool
#endif