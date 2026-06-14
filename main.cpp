#include <iostream>
#include <chrono>
#include "ModernThreadPool/ThreadPool/ThreadPool.h"

int sumFunc(int begin=0, int end=0) {
    int sum = 0;
    for (int i=begin; i<end; ++i) {
        sum += i;
    }
    return sum;
}

int main() {
    std::future<int> res1 = ThreadPool::instance().commit(sumFunc, 0, 500);
    std::future<int> res2 = ThreadPool::instance().commit(sumFunc, 50, 1000);
    std::future<int> res3 = ThreadPool::instance().commit(sumFunc, 1000, 2000);
    std::cout << res1.get() << std::endl;
    std::cout << res2.get() << std::endl;
    std::cout << res3.get() << std::endl;
}