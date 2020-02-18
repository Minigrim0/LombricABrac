#include <iostream>

#include "../includes/connected_player.hpp"
#include "../includes/database.hpp"

ConnectedPlayer::ConnectedPlayer():m_is_auth(false){}

ConnectedPlayer::~ConnectedPlayer(){}

void ConnectedPlayer::set_username(std::string username){
    m_name = username;
}

bool ConnectedPlayer::is_auth() const{
    return m_is_auth;
}

bool ConnectedPlayer::register_user(DataBase* db, std::string passwd){
    passwd = hash_passwd(passwd);
    db->register_user(m_name, passwd);
    return db->get_las();
}

bool ConnectedPlayer::check_passwd(DataBase* db, std::string passwd) const{
    passwd = hash_passwd(passwd);
    db->get_passwd(m_name);
    std::string stored_passwd = db->get_last_out();

    return db->get_las();
}

std::string ConnectedPlayer::hash_passwd(std::string passwd) const{
    return passwd;
}
