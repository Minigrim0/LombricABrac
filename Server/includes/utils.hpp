#ifndef utils_hpp
    #define utils_hpp

#include <mutex>
#include <condition_variable>

#include "listener.hpp"
#include "database.hpp"
#include "connected_player.hpp"
#include "zhelpers.hpp"

#define CHAT_BROKER 100

// Database needs
extern std::mutex DataBase_mutex;
extern DataBase db;

// ZMQ needs
extern std::mutex pub_mutex;
extern zmq::socket_t publisher;

void catch_error(int res, int is_perror, const char* msg, int nb_to_close, ...);
int handle_instruction(uint8_t msg_type, Listener* la_poste, ConnectedPlayer* usr, std::string zmq_msg);
void create_room_thread(ZMQ_msg zmqmsg);

// Cases
bool send_room_invite(ZMQ_msg *zmqmsg, Listener *la_poste, ConnectedPlayer *usr);
int register_user(Listener* la_poste, ConnectedPlayer *usr);
void ask_friendship_list(Listener* la_poste, ConnectedPlayer* usr);

#endif
