#include <iostream>

#include "../includes/shared_memory.hpp"

std::mutex* mu = new std::mutex;

void shared_cout(std::string msg, int id){
	mu->lock();
	std::cout << msg << ":" << id << std::endl;
	mu->unlock();
}
