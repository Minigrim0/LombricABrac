#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/game_thread.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../cpl_proto/user.pb.h"

int game_thread(int user1_id, int user2_id, int user3_id, int user4_id, DataBase* db){

    Game current_game;
    int current_step = STEP_ROOM;

    current_game.set_is_running(true);
    current_game.set_player1_id(-1);
    current_game.set_player2_id(-1);
    current_game.set_player3_id(-1);
    current_game.set_player4_id(-1);
    current_game.set_points_1(0);
    current_game.set_points_2(0);
    current_game.set_points_3(0);
    current_game.set_points_4(0);

    while(current_game.is_running()){
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
                current_game.set_is_running(false);
        }
    }

    return EXIT_SUCCESS;
}
