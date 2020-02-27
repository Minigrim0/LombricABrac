#ifndef utils_hpp
    #define utils_hpp

#include <mutex>
#include <condition_variable>

#include "listener.hpp"
#include "database.hpp"
#include "connected_player.hpp"
#include "zhelpers.hpp"

#define CHAT_BROKER 100

extern int match_queue[5];
extern int nb_waiting_players;
extern std::mutex mu;
extern std::condition_variable cv;
extern std::mutex DataBase_mutex;
extern DataBase db;
extern std::mutex pub_mutex;
extern zmq::context_t context;
extern zmq::socket_t publisher;

static Semaphore sem;
static std::mutex match_queue_mut;

void add_me_to_queue(int user_id);

void catch_error(int res, int is_perror, const char* msg, int nb_to_close, ...);
int handle_instruction(uint8_t msg_type, Listener* la_poste, ConnectedPlayer* usr, std::string zmq_msg);

#endif
