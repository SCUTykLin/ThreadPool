//
// Created by YangKai.Lin on 2026/6/14.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(int num_thread) : stop_(false) {
    this->thread_num_ = num_thread <= 1 ? 2 : num_thread;
    start();
}

void ThreadPool::start() {
    for (int i = 0; i < thread_num_; ++i) {
        pool_.emplace_back(
            [this]()->void {
               while (!this->stop_.load()) {
                   std::packaged_task<void()> task;
                   {
                       std::unique_lock<std::mutex> lock(cv_mt_);
                       cv_lock_.wait(lock, [this]()->bool {
                           return !task_.empty() || this->stop_.load();
                       });
                       if (task_.empty()) {
                           return;
                       }
                       task = std::move(task_.front());
                       this->task_.pop();
                   }
                   std::cout << "start thread: " << std::this_thread::get_id() << std::endl;
                   this->thread_num_--;
                   task();
                   this->thread_num_++;
                   std::cout << "end thread: " << std::this_thread::get_id() << std::endl;
               }
            });
    }
}

void ThreadPool::stop() {
    stop_.store(true);
    cv_lock_.notify_all();
    for (std::thread& t : pool_) {
        if (t.joinable()) {
            t.join();
        }
    }
    std::cout << "Stop ThreadPool" << std::endl;
}

int ThreadPool::idleThreadCount() {
    return thread_num_;
}

ThreadPool::~ThreadPool() {
    stop();
}
