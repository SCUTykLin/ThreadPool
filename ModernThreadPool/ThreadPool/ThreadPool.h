//
// Created by YangKai.Lin on 2026/6/14.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include "../NoneCopy/NoneCopy.h"
#include <mutex>
#include <functional>
#include <future>
#include <queue>
#include <iostream>

class ThreadPool : public NoneCopy{
public:

    template<typename F, typename... Args>
    auto commit(F&& f, Args&& ... args)->std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> {
        using return_type = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        if (stop_.load(std::memory_order_relaxed)) {
            return std::future<return_type>();
        }
        std::function<return_type()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task = std::make_shared<std::packaged_task<return_type()>>(func);
        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(cv_mt_);
            task_.emplace([task](){(*task)();});
        }
        cv_lock_.notify_one();
        return res;
    }

    int idleThreadCount();
    ~ThreadPool();
    static ThreadPool& instance() {
        static ThreadPool ins;
        return ins;
    }
    ThreadPool(int num_thread = std::thread::hardware_concurrency());
private:
    void start();
    void stop();

    std::mutex cv_mt_;
    std::condition_variable cv_lock_;
    std::atomic_bool stop_;
    std::atomic_int thread_num_;
    std::queue<std::packaged_task<void()>> task_;
    std::vector<std::thread> pool_;
};


#endif //THREADPOOL_THREADPOOL_H