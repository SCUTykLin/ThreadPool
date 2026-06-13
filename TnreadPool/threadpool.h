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
#include <functional>

class Any;
class Semaphore;
class Result;
class Task;

class Any {
public:
    Any() = default;
    ~Any() = default;
    Any(const Any &other) = delete;
    Any& operator=(const Any &other) = delete;
    Any(Any &&other) = default;
    Any& operator=(Any &&other) = default;

    template <typename T>
    Any(T&& data) : base_(std::make_unique<Derive<T>>(std::forward<T>(data))){}

    template<typename T>
    T& cast_() {
        Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
        if (pd == nullptr) {
            throw std::bad_cast();
        }
        return pd->data_;
    }

private:
    class Base {
    public:
        virtual ~Base() = default;
    };
    template <typename T>
    class Derive : public Base {
    public:
        Derive(T&& data) : data_(std::forward<T>(data)) {}
        T data_;
    };
private:
    std::unique_ptr<Base> base_;
};

enum class PoolMode {
    MODE_FIXED,
    MODE_CACHE
};

class Thread {
public:
    using ThreadFunc = std::function<void()>;

    Thread(ThreadFunc func);

    ~Thread();

    void start();
private:
    ThreadFunc func_;
};


class Semaphore {
public:
    Semaphore(int limit = 0) : resLimit_(limit) {}
    ~Semaphore() {}
    void wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [this]() { return resLimit_ > 0; });
        resLimit_--;
    }
    void post() {
        std::unique_lock<std::mutex> lock(mtx_);
        resLimit_ ++;
        cond_.notify_all();
    }
private:
    int resLimit_;
    std::mutex mtx_;
    std::condition_variable cond_;
};

class Result {
public:
    Result(std::shared_ptr<Task> task, bool isValid=true);
    ~Result() = default;

    Any get();
    void setVal(Any any);
private:
    Any any_;
    Semaphore sem_;
    std::shared_ptr<Task> task_;
    std::atomic_bool isValid_;
};

class Task {
public:
    Task():result_(nullptr){}
    ~Task() = default;
    virtual Any run() = 0;
    void exec();
    void setResult(Result* result);
private:
    Result* result_;
};
inline void Task::exec() {
    if (result_ != nullptr) {
        result_->setVal(run());
    }
}

class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    void setMode(PoolMode mode);
    void setTaskQueMaxThreshHold(int threshhold);
    void setInitThreadSize(int size);
    Result submitTask(std::shared_ptr<Task> sp);
    void start(int initThreadSize = std::thread::hardware_concurrency());

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void threadFunc();

private:
    std::vector<std::unique_ptr<Thread>> threads_; // list of threads
    size_t initThreadSize_; // #threads
    std::queue<std::shared_ptr<Task>> taskQue_;
    std::atomic_uint taskSize_;
    std::size_t taskQueThreshHold_; // Max #Tasks

    std::mutex taskQueMtx_; // Saft lock of queue
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;

    PoolMode poolMode_;
};


#endif //THREADPOOL_THREADPOOL_H