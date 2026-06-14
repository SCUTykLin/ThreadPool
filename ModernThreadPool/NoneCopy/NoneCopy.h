//
// Created by YangKai.Lin on 2026/6/14.
//

#ifndef THREADPOOL_NONECOPY_H
#define THREADPOOL_NONECOPY_H


class NoneCopy {
public:
    ~NoneCopy() = default;
protected:
    NoneCopy() = default;
private:
    NoneCopy(const NoneCopy&) = delete;
    NoneCopy& operator=(const NoneCopy&) = delete;
};


#endif //THREADPOOL_NONECOPY_H