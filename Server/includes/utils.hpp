#ifndef utils_hpp
    #define utils_hpp

#include "listener.hpp"
#include "database.hpp"
#include "connected_player.hpp"

void catch_error(int res, int is_perror, const char* msg, int nb_to_close, ...);
void handle_instruction(int msg_type, Listener* yolo, DataBase* db, ConnectedPlayer* usr);

#endif
