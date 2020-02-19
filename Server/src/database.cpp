#include <iostream>
#include <sstream>

#include "../includes/database.hpp"
#include "../cpl_proto/user.pb.h"

DataBase::DataBase(std::string db_path):m_is_open(false){
    m_rc = sqlite3_open(db_path.c_str(), &m_db);
    if(m_rc){
        std::cout << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        m_is_open = false;
    }
    else{
        std::cout << "Database opened" << std::endl << std::endl;
        m_is_open = true;
    }
}

DataBase::~DataBase(){
    sqlite3_close(m_db);
}

// Getters
bool DataBase::is_opened() const{
    return m_is_open;
}

bool DataBase::catch_error(){
    if(m_rc != SQLITE_OK){
       std::cout << "SQL error: " << m_zErrMsg << std::endl;
       sqlite3_free(m_zErrMsg);
       return false;
    }
    std::cout << "Request successfull" << std::endl;
    return true;
}

int DataBase::callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", static_cast<const char*>(data));

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}

bool DataBase::get_las() const{
    return m_rc;
}

std::string DataBase::get_last_out() const{
    return "";
}

// User operations
UserConnect DataBase::get_user(std::string username){
    m_sql_request = "SELECT id, username, victory_amount from users where username='" + username + "';";
    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

std::string DataBase::get_passwd(std::string username){
    m_sql_request = "SELECT password from users where username='" + username + "';";
    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

bool DataBase::register_user(std::string username, std::string password){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO users (username,password,victory_amount)";
    m_stringStream << "VALUES (" << username << ", " << password << ", 0);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}


// Lombrics Operations
bool DataBase::add_lombric(int user_id, std::string lombric_name){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO worms (name,owner_id)";
    m_stringStream << "VALUES (" << lombric_name << ", " << user_id << ", 0);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

bool DataBase::set_lombric_name(int lombric_id, std::string name){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "UPDATE worms SET name=" << name << " WHERE id=" << lombric_id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Lomb_r DataBase::get_lombrics(int owner_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT name FROM worms WHERE owner_id=" << owner_id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}


// Game history operations
History_r DataBase::get_history(std::string username, int index, int size){

}

bool DataBase::add_history_entry(int player1, int player2, int player3, int player4, int player1_points, int player2_points, int player3_points, int player4_points){}


// Game rank operations
Rank_r DataBase::get_rank(int index, int size){}


// Message operations
bool DataBase::send_message(int sender_id, int receiver_id, std::string message){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO messages (sender_id, receiver_id, content) ";
    m_stringStream << "VALUES (" << sender_id << ", " << receiver_id << ", '" << message << "'); ;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Chat_r DataBase::get_received_messages(int user_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT content, sender_id FROM messages ";
    m_stringStream << "WHERE receiver_id=" << user_id << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Chat_r DataBase::get_sent_messages(int user_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT content, receiver_id FROM messages ";
    m_stringStream << "WHERE sender_id=" << user_id << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Chat_r DataBase::get_all_messages(int user_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT content, receiver_id, sender_id FROM messages ";
    m_stringStream << "WHERE sender_id=" << user_id << " OR receiver_id=" << user_id;
    m_stringStream << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}


// Friendship operations
bool DataBase::add_friend(int sender_id, int receiver_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO friends (sender_id, receiver_id, accepted)";
    m_stringStream << " VALUES (" << sender_id << ", " << receiver_id << ", false);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Fri_ls_r DataBase::get_friend_list(int user_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT * FROM friends";
    m_stringStream << " WHERE (sender_id=" << user_id << " AND accepted=true) OR (receiver_id=" << user_id << " AND accepted=true);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Fri_ls_r DataBase::get_all_friend_list(int user_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT * FROM friends";
    m_stringStream << " WHERE sender_id=" << user_id << " OR receiver_id=" << user_id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

Fri_ls_r DataBase::get_friend_invites(int user_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT * FROM friends";
    m_stringStream << " WHERE receiver_id=" << user_id << " AND accepted=false;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

bool DataBase::accept_friend_invite(int id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "UPDATE friends SET accepted=true WHERE id=" << id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}

bool DataBase::remove_friend(int id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "DELETE FROM friends WHERE id=" << id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
}
