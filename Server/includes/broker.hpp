#ifndef broker_hpp
    #define broker_hpp


#include "../proto/src/user.pb.h"

int broker_thread();

// Broker Utils
void ping_rooms();
void create_room_thread(ZMQ_msg zmqmsg);

#endif
