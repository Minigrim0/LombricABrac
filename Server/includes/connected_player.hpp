#ifndef ConnectedPlayer_hpp
    #define ConnectedPlayer_hpp

#include <iostream>

#include "../includes/database.hpp"

class ConnectedPlayer{
    public:
        ConnectedPlayer();
        ~ConnectedPlayer();

        // Setter
        void set_username(std::string username);

        // Getters
        bool is_auth() const;

        // Others
        bool register_user(DataBase* db, std::string passwd);
        bool check_passwd(DataBase* db, std::string passwd) const;

        std::string hash_passwd(std::string passwd) const;

    private:
        std::string m_name;
        bool m_is_auth;
};

#endif
