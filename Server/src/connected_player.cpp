#include <iostream>

#include "../includes/connected_player.hpp"
#include "../includes/database.hpp"
#include "../lib/bcrypt.h"

ConnectedPlayer::ConnectedPlayer()
:UserConnect(),
m_is_auth(false),
m_id(-1)
{}

ConnectedPlayer::~ConnectedPlayer(){}

bool ConnectedPlayer::is_auth() const{
    return m_is_auth;
}

int ConnectedPlayer::get_id() const{
    return m_id;
}

void ConnectedPlayer::set_id(int id){
    m_id = id;
}

void ConnectedPlayer::set_auth(bool auth){
    m_is_auth = auth;
}

bool ConnectedPlayer::register_user(DataBase* db, std::string passwd){
    return db->register_user(pseudo(), passwd) == SQLITE_OK;
}

bool ConnectedPlayer::check_passwd(DataBase* db, std::string passwd) const{
    std::string stored_passwd;
    db->get_passwd(pseudo(), &stored_passwd);

    return (bcrypt_checkpw(passwd.c_str(),stored_passwd.c_str()) == 0);
}
