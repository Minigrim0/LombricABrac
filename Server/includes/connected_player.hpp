#ifndef ConnectedPlayer_hpp
    #define ConnectedPlayer_hpp

#include <iostream>

#include "../includes/database.hpp"
#include "../cpl_proto/user.pb.h"

class ConnectedPlayer: public UserConnect{
    public:
        ConnectedPlayer();
        ~ConnectedPlayer();

        // Getters
        bool is_auth() const;
        int get_id() const;

        void set_id(int id);
        void set_auth(bool auth);

        // Others
        bool register_user(DataBase* db, std::string passwd);
        bool check_passwd(DataBase* db, std::string passwd) const;

    private:
        bool m_is_auth;
        int m_id;
};

#endif
