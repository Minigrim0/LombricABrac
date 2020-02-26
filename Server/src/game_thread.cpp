#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/game_thread.hpp"
#include "../includes/game.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../cpl_proto/user.pb.h"

int game_thread(std::string chan_sub){

    bool game_running;
    int current_step = STEP_ROOM;
    Game current_game;

    while(game_running){
        switch(current_step){
            case STEP_ROOM:
                std::cout << "You're in the room" << std::endl;
                break;
            case STEP_GAME:
                std::cout << "You're in the game" << std::endl;
                current_game.set_begin();
                while(game_running){
                    if(current_game.check_time()){
                        //mort subite
                    }

                }
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
