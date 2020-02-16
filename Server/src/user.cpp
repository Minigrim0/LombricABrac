#include <iostream>
#include <unistd.h>

#include "../cpl_proto/user.pb.h"

UserConnect setUser(){
    UserConnect user;
    user.set_isregister(true);
    user.set_pseudo("morti");
    user.set_password("abc");
    std::cout << user.DebugString() << std::endl;
    return user;
}
