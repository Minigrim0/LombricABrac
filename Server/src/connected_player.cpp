#include <iostream>

#include "../includes/connected_player.hpp"
#include "../includes/database.hpp"

ConnectedPlayer::ConnectedPlayer()
:UserConnect(),
m_is_auth(false){}

ConnectedPlayer::~ConnectedPlayer(){}

bool ConnectedPlayer::is_auth() const{
    return m_is_auth;
}

bool ConnectedPlayer::register_user(DataBase* db, std::string passwd){
    passwd = hash_passwd(passwd);
    db->register_user(pseudo(), passwd);
    return db->get_las();
}

bool ConnectedPlayer::check_passwd(DataBase* db, std::string passwd) const{
    passwd = hash_passwd(passwd);
    db->get_passwd(pseudo());
    std::string stored_passwd = db->get_last_out();

    return db->get_las();
}

std::string ConnectedPlayer::hash_passwd(std::string passwd) const{
    return passwd;
}
