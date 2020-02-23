#ifndef shared_memory_hpp
    #define shared_memory_hpp

#include <mutex>

extern std::mutex* mu;
void shared_cout(std::string msg, int id);

#endif
