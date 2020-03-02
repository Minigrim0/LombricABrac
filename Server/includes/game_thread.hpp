#ifndef game_thread_hpp
    #define game_thread_hpp

#include "database.hpp"

#define STEP_ROOM 0
#define STEP_GAME 1
#define STEP_ENDSCREEN 2

int game_thread(std::string chan_sub, uint32_t owner_id);

#endif
