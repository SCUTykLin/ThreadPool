//
// Created by YangKai.Lin on 2026/6/10.
//

#include "threadpool.h"
#include <thread>
#include <functional>
#include <iostream>

const int TASK_MAX_THRESHHOLD = 1024;

void Task::setResult(Result* result) {
    result_ = result;
}


Thread::Thread(ThreadFunc func) : func_(func) {
}

Thread::~Thread() {
}

void Thread::start() {
    std::thread t(func_);
    t.detach(); // 要不然t出来就挂了
}

Result::Result(std::shared_ptr<Task> task, bool isValid) : task_(task), isValid_(isValid) {
    task_->setResult(this);
}

Any Result::get() {
    if (!isValid_) {
        return nullptr;
    }
    sem_.wait();
    return std::move(any_);
}

void Result::setVal(Any any) {
    this->any_ = std::move(any);
    sem_.post();
}

ThreadPool::ThreadPool() : initThreadSize_(4), taskQueThreshHold_(TASK_MAX_THRESHHOLD), taskSize_(0),  poolMode_(PoolMode::MODE_FIXED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode) {
    poolMode_ = mode;
}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold) {
    taskQueThreshHold_ = threshhold;
}

void ThreadPool::setInitThreadSize(int size) {
    initThreadSize_ = size;
}

void ThreadPool::start(int initThreadSize) {
    initThreadSize_ = initThreadSize;
    for (int i = 0; i < initThreadSize; i++) {
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        threads_.emplace_back(std::move(ptr));
    }

    for (int i = 0; i < initThreadSize; i++) {
        threads_[i]->start();
    }
}

void ThreadPool::threadFunc() {
    for (;;) {
        // access lock
        std::shared_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lock(taskQueMtx_);
            // wait until notempty
            notEmpty_.wait(lock, [this]()->bool{return !taskQue_.empty();});
            // obtain task from queue
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;

            if (!taskQue_.empty()) {
                notEmpty_.notify_all();
            }
            notFull_.notify_all();
        }
        // run task
        if (task != nullptr) {
            task->exec();
        }
    }
}

Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    if (!notFull_.wait_for(lock, std::chrono::seconds(1), [this]()->bool{return taskQue_.size() < taskQueThreshHold_;})) {
        std::cerr << "submitTask timeout" << std::endl;
        return Result(sp, false);
    }
    taskQue_.emplace(sp);
    taskSize_++;
    notEmpty_.notify_all();
}

