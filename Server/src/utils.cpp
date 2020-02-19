#include <iostream>
#include <unistd.h>
#include <cstdarg>

#include "../includes/comm_macros.hpp"
#include "../includes/listener.hpp"
#include "../includes/database.hpp"
#include "../cpl_proto/user.pb.h"


// Depending on the value of res, indicate an error, with the error message provided,
// and closes given pipes before exiting the program
void catch_error(int res, int is_perror, const char* msg, int nb_to_close, ...){
    if(res == -1){
        if(is_perror == 1){
            perror(msg);
        }
        else{
            std::cout << msg << std::endl;
        }

        if(nb_to_close > 0){
            va_list ap;
            va_start(ap, nb_to_close);

            int i;
            // traverse rest of the arguments to find out minimum
            for(i = 2; i <= nb_to_close; i++)
                close(va_arg(ap, int));
        }
        exit(EXIT_FAILURE);
    }
}

void handle_case(int msg_type, Listener* yolo , DataBase* db){
    if(msg_type == CON_S){
        UserConnect usr;
        yolo->reception();
        usr.ParseFromString(yolo->get_buffer());
        if(usr.isregister() == true){
            db->verify_user(usr.pseudo(),usr.password());
        }
    }
}
