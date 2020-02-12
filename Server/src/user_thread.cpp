#include <iostream>

#include "../includes/user_thread.hpp"
#include "../includes/shared_memory.hpp"

void client_thread(){
    for(int i=0;i<20;i++)
        shared_cout("thread function", i);
}
