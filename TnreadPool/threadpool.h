//
// Created by YangKai.Lin on 2026/6/10.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <vector>
#include <queue>
#include <functional>
#include <atomic>

enum PoolMode {
    MODE_FIXED,
    MODE_CACHE
};

template<typename Task>
class BaseTask {
public:
    void run() {
        static_cast<Task*>(this)->runImpl();
    }
};

class Thread {

};

class ThreadPool {
public:
private:
    std::vector<Thread*> threads_; // list of threads
    size_t initThreadSize_; // #threads
    std::queue<std::function<void()>> taskQue_;
    std::atomic_uint 
};


#endif //THREADPOOL_THREADPOOL_H