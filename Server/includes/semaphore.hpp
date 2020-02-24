#ifndef semaphore_hpp
    #define semaphore_hpp

#include <iostream>
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore (int count_ = 0)
        : count(count_) {}

    inline void notify()
    {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        std::cout << "notohsrilygzfikgnhrtoiuhnzerkgfkerngckierucgkeru" << std::endl;
        cv.notify_one();
        std::cout << "notohsrilygzfikgnhrtoiuhnzerkgfkerngckierucgkeru" << std::endl;
    }

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(mtx);

        while(count == 0){
            std::cout << "a" << std::endl;
            cv.wait(lock);
            std::cout << "b" << std::endl;
        }
        count--;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

#endif
