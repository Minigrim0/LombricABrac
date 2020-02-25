#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../cpl_proto/user.pb.h"

int game_thread(){

    bool game_running;
    int current_step = STEP_ROOM;


    while(game_running){
        switch(current_step){
            case STEP_ROOM:
                std::cout << "You're in the room" << std::endl;
                break;
            case STEP_GAME:
                std::cout << "You're in the game" << std::endl;
                break;
            case STEP_ENDSCREEN:
                std::cout << "You're in the end screen" << std::endl;
                break;
            default:
                std::cout << "An error occured" << std::endl;
                game_running = false;
        }
    }

    return EXIT_SUCCESS;
}
