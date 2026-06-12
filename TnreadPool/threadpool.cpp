//
// Created by YangKai.Lin on 2026/6/10.
//

#include "threadpool.h"
#include <thread>
#include <functional>

const int TASK_MAX_THRESHHOLD = 1024;

void Thread::start() {

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
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::threadFunc, this)));
    }

    for (int i = 0; i < initThreadSize; i++) {
        threads_[i]->start();
    }
}

void ThreadPool::threadFunc() {

}

void ThreadPool::submitTask() {
}
