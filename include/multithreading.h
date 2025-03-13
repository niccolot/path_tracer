#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <mutex>
#include <condition_variable>
#include <queue>

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