cmake_minimum_required(VERSION 4.0)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

set(Modern
        ModernThreadPool/NoneCopy/NoneCopy.cpp
        ModernThreadPool/NoneCopy/NoneCopy.h
        ModernThreadPool/ThreadPool/ThreadPool.cpp
        ModernThreadPool/ThreadPool/ThreadPool.h)

add_executable(ThreadPool main.cpp
#        TnreadPool/threadpool.cpp
#        TnreadPool/threadpool.h
        ${Modern})