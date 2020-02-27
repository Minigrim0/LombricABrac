#include <iostream>
#include <thread>
#include <unistd.h>

#include "../includes/game_thread.hpp"
#include "../includes/game.hpp"
#include "../includes/listener.hpp"
#include "../includes/utils.hpp"
#include "../cpl_proto/user.pb.h"

int game_thread(std::string chan_sub, uint32_t owner_id){

    bool game_running;
    int current_step = STEP_ROOM;
    Game current_game;
    
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, chan_sub.c_str(), strlen(chan_sub.c_str()));

    while(game_running){
        switch(current_step){
            case STEP_ROOM:{
                std::cout << "You're in the room" << std::endl;
                ZMQ_msg zmqmsg;
                std::ostringstream stream;
                while (current_step == STEP_ROOM)
                {
                    std::string address = s_recv(subscriber);
                    std::string contents = s_recv(subscriber);

                    zmqmsg.ParseFromString(contents);
                    current_game.handle_room(zmqmsg ,&current_step);
                }
                
                break;
            }
            case STEP_GAME:
                std::cout << "You're in the game" << std::endl;
                //current_game.set_begin();
                while(current_step == STEP_GAME){
                    //if(current_game.check_time()){
                    //    //mort subite                 WIP
                    //}

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
