#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>

template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable cv;

public:
    ThreadSafeQueue() {}

    void wait_and_pop(T& val) {
        std::unique_lock<std::mutex> lk(mut);
        cv.wait(lk, [this] { return !data_queue.empty(); });
        val = std::move(data_queue.front());
        data_queue.pop();
    }

    bool try_pop(T& val) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) {
            return false;
        }

        val = std::move(data_queue.front());
        data_queue.pop();

        return true;
    }

    T wait_and_pop() {
        std::unique_lock<std::mutex> lk(mut);
        cv.wait(lk, [this] { return !data_queue.empty(); });
        T res = std::move(data_queue.front());
        data_queue.pop();

        return res;
    }

    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) {
            return std::optional<T>();
        }

        std::optional<T> res = std::move(data_queue.front());
        data_queue.pop();

        return res;
    }

    void push(T&& val) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(val));
        cv.notify_one();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);

        return data_queue.empty();
    }
}; // class ThreadSafeQueue 

typedef struct ScanLine {
    uint32_t row;
    std::vector<uint32_t> values;
} scanline_t;

//void worker_task(Camera& cam, ThreadSafeQueue<scanline_t>& queue) {
//    uint32_t row = 0;
//    
//    while (!quit_app)
//    {
//        // sleep for few millisecond
//        std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });        
//        queue.push({ row, std::move(cam.render_row(row)) });
//        
//        if (row == height) {
//            done_rendering = true;
//        }
//        row++;
//    }
//}
#endif