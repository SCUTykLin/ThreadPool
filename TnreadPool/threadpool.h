//
// Created by YangKai.Lin on 2026/6/10.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <vector>
#include <queue>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
enum class PoolMode {
    MODE_FIXED,
    MODE_CACHE
};

class Thread {
public:
    void start();
};

template<typename Task>
class BaseTask {
public:
    void run() {
        static_cast<Task*>(this)->runImpl();
    }

    std::vector<std::function<void()>> func_que;
};
class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    void setMode(PoolMode mode);
    void setTaskQueMaxThreshHold(int threshhold);
    void setInitThreadSize(int size);
    void submitTask();
    void start(int initThreadSize = std::thread::hardware_concurrency());

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void threadFunc();

private:
    std::vector<Thread*> threads_; // list of threads
    size_t initThreadSize_; // #threads
    std::queue<std::function<void()>> taskQue_;
    std::atomic_uint taskSize_;
    int taskQueThreshHold_; // Max #Tasks

    std::mutex taskQueMtx_; // Saft lock of queue
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;

    PoolMode poolMode_;
};


#endif //THREADPOOL_THREADPOOL_H