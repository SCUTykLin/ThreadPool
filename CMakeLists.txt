cmake_minimum_required(VERSION 4.0)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

add_executable(ThreadPool main.cpp
        TnreadPool/threadpool.cpp
        TnreadPool/threadpool.h)