#include <iostream>
#include <sstream>

#include "../includes/database.hpp"
#include "../cpl_proto/user.pb.h"
#include "../lib/bcrypt.h"

// Static attribute must be declared before class methods
uint8_t DataBase::m_data_type;

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

int DataBase::callback(void *data_container, int argc, char **argv, char **azColName){
    switch(m_data_type){
        case DT_USR:
            for(int i = 0; i<argc; i++){
                if(strcmp(azColName[i], "username") == 0){
                    static_cast<UserConnect*>(data_container)->set_pseudo(argv[i]);
                }
            }
            break;
        case DT_STR:
            *static_cast<std::string*>(data_container) = argv[0];
            break;
        case DT_INT:
            *static_cast<int*>(data_container) = std::stoi(argv[0]);
            break;
        case DT_LOM:
            static_cast<Lomb_r*>(data_container)->set_lomb_1(argv[0]);
            static_cast<Lomb_r*>(data_container)->set_lomb_2(argv[1]);
            static_cast<Lomb_r*>(data_container)->set_lomb_3(argv[2]);
            static_cast<Lomb_r*>(data_container)->set_lomb_4(argv[3]);
            static_cast<Lomb_r*>(data_container)->set_lomb_5(argv[4]);
            static_cast<Lomb_r*>(data_container)->set_lomb_6(argv[5]);
            static_cast<Lomb_r*>(data_container)->set_lomb_7(argv[6]);
            static_cast<Lomb_r*>(data_container)->set_lomb_8(argv[7]);
            break;
        case DT_HIS:{
            History *history = static_cast<History_r*>(data_container)->add_history();

            for(int i=0;i<argc;i++){
                if(strcmp(azColName[i], "user_1_id") == 0){
                    history->set_pseudo_1(argv[i]);
                }
                else if(strcmp(azColName[i], "user_2_id") == 0){
                    history->set_pseudo_2(argv[i]);
                }
                else if(strcmp(azColName[i], "user_3_id") == 0){
                    history->set_pseudo_3(argv[i]);
                }
                else if(strcmp(azColName[i], "user_4_id") == 0){
                    history->set_pseudo_4(argv[i]);
                }
                else if(strcmp(azColName[i], "user_1_points") == 0){
                    history->set_point_1(std::stoi(argv[i]));
                }
                else if(strcmp(azColName[i], "user_2_points") == 0){
                    history->set_point_2(std::stoi(argv[i]));
                }
                else if(strcmp(azColName[i], "user_3_points") == 0){
                    history->set_point_3(std::stoi(argv[i]));
                }
                else if(strcmp(azColName[i], "user_4_points") == 0){
                    history->set_point_4(std::stoi(argv[i]));
                }
                else if(strcmp(azColName[i], "timestamp") == 0){
                    history->set_date(std::stoi(argv[i]));
                }
            }
            break;
        }
        case DT_RAN:
            for(int i = 0; i<argc; i++){
                if(strcmp(azColName[i], "username") == 0){
                    std::string *username = static_cast<Rank_r*>(data_container)->add_user();
                    *username = argv[i];
                }
                else if(strcmp(azColName[i], "victory_amount") == 0){
                    static_cast<Rank_r*>(data_container)->add_point(std::stoi(argv[i]));
                }
            }
            break;
        case DT_CHA:
            for(int i = 0; i<argc; i++){
                if(strcmp(azColName[i], "content") == 0){
                    Chat *chat = static_cast<Chat_r*>(data_container)->add_msgs();
                    chat->set_msg(argv[i]);
                }
            }
            break;
        case DT_FRI:
            for(int i = 0; i<argc; i++){
                std::string* str = static_cast<Fri_ls_r*>(data_container)->add_user();
                *str = argv[i];
            }
            break;
        default:
            std::cout << "> Error, datatype not recognised" << std::endl;
    }

    return 0;
}

bool DataBase::get_las() const{
    return m_rc;
}


// User operations
int DataBase::get_user(std::string username, UserConnect* userconnect){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_USR;

    m_sql_request = "SELECT id, username, victory_amount FROM users WHERE username='" + username + "';";
    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, userconnect, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_user(int user_id, UserConnect* userconnect){
    m_stringStream.str("");
    m_stringStream.clear();
    m_stringStream << "SELECT id, username, victory_amount FROM users WHERE id='" << user_id << "';";
    m_sql_request = m_stringStream.str();

    m_data_type = DT_USR;

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, userconnect, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_passwd(std::string username, std::string* password){
    m_data_type = DT_STR;

    m_sql_request = "SELECT password FROM users WHERE username='" + username + "';";
    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, password, &m_zErrMsg);

    return m_rc;
}

int DataBase::register_user(std::string username, std::string password){
    m_stringStream.str("");
    m_stringStream.clear();
    bcrypt_gensalt(12,salt);
    bcrypt_hashpw(password.c_str(),salt,hash);

    m_stringStream << "INSERT INTO users (username,password,victory_amount)";
    m_stringStream << "VALUES ('" << username << "', '" << hash << "', 0);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);
    return m_rc;
}

int DataBase::get_user_id(std::string username, int* id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_INT;

    m_sql_request = "SELECT id FROM users WHERE username='" + username + "';";
    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, id, &m_zErrMsg);

    return m_rc;
}


// Lombrics Operations
int DataBase::add_lombric(int user_id, std::string lombric_name){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO worms (name,owner_id)";
    m_stringStream << "VALUES (" << lombric_name << ", " << user_id << ", 0);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}

int DataBase::set_lombric_name(int lombric_id, std::string name){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "UPDATE worms SET name=" << name << " WHERE id=" << lombric_id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_lombrics(int owner_id, Lomb_r* lomb_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_LOM;

    m_stringStream << "SELECT name FROM worms WHERE owner_id=" << owner_id << ";";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, lomb_r, &m_zErrMsg);

    return m_rc;
}


// Game history operations
int DataBase::get_history(int user_id, int index, int size, History_r* history_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT * FROM history WHERE (user_1_id="
                   << user_id
                   << " OR user_2_id="
                   << user_id
                   << " OR user_3_id="
                   << user_id
                   << " OR user_4_id="
                   << user_id << ") ORDER BY timestamp DESC LIMIT "
                   << index << ", " << size << ";";
    m_sql_request = m_stringStream.str();

    m_data_type = DT_HIS;

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, history_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::add_history_entry(int player1, int player2, int player3, int player4, int player1_points, int player2_points, int player3_points, int player4_points){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO history (user_1_id, user_2_id, user_3_id, user_4_id, user_1_points, user_2_points, user_3_points, user_4_points) "
                   << "VALUES (" << player1 << ", " << player2 << ", " << player3 << ", " << player4 << ", "
                   << player1_points << ", " << player2_points << ", " << player3_points << ", " << player4_points << "); ";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}


// Game rank operations
int DataBase::get_rank(int index, int size, Rank_r* rank_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "SELECT username, victory_amount FROM users ORDER BY victory_amount DESC LIMIT ";
    m_stringStream << index << ", " << size << ";";
    m_sql_request = m_stringStream.str();

    m_data_type = DT_RAN;

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, rank_r, &m_zErrMsg);

    return m_rc;
}


// Message operations
int DataBase::send_message(int sender_id, int receiver_id, std::string message){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO messages (sender_id, receiver_id, content) ";
    m_stringStream << "VALUES (" << sender_id << ", " << receiver_id << ", '" << message << "'); ;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_received_messages(int user_id, Chat_r* chat_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_CHA;

    m_stringStream << "SELECT content, sender_id FROM messages ";
    m_stringStream << "WHERE receiver_id=" << user_id << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, chat_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_sent_messages(int user_id, Chat_r* chat_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_CHA;

    m_stringStream << "SELECT content, receiver_id FROM messages ";
    m_stringStream << "WHERE sender_id=" << user_id << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, chat_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_all_messages(int user_id, Chat_r* chat_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_CHA;

    m_stringStream << "SELECT content, receiver_id, sender_id FROM messages ";
    m_stringStream << "WHERE sender_id=" << user_id << " OR receiver_id=" << user_id;
    m_stringStream << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, chat_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_messages_sent_to(int sender_id, int receiver_id, Chat_r* chat_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_CHA;

    m_stringStream << "SELECT content, receiver_id, sender_id FROM messages ";
    m_stringStream << "WHERE sender_id=" << sender_id << " AND receiver_id=" << receiver_id;
    m_stringStream << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, chat_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_messages_received_from(int receiver_id, int sender_id, Chat_r* chat_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_CHA;

    m_stringStream << "SELECT content, receiver_id, sender_id FROM messages ";
    m_stringStream << "WHERE sender_id=" << sender_id << " OR receiver_id=" << receiver_id;
    m_stringStream << " ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, chat_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_convo(int user1_id, int user2_id, Chat_r* chat_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_CHA;

    m_stringStream << "SELECT content, receiver_id, sender_id FROM messages ";
    m_stringStream << "WHERE (sender_id=" << user1_id << " AND receiver_id=";
    m_stringStream << user2_id << ") OR (sender_id=" << user2_id;
    m_stringStream << " AND receiver_id=" << user1_id;
    m_stringStream << ") ORDER BY timestamp;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, chat_r, &m_zErrMsg);

    return m_rc;
}


// Friendship operations
int DataBase::add_friend(int sender_id, int receiver_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_FRI;

    m_stringStream << "INSERT INTO friends (sender_id, receiver_id, accepted)";
    m_stringStream << " VALUES (" << sender_id << ", " << receiver_id << ", false);";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_friend_list(int user_id, Fri_ls_r* fri_ls_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_FRI;

    m_stringStream << "SELECT username FROM friends, users WHERE friends.sender_id="
                   << user_id
                   << " AND users.id=friends.receiver_id and accepted=true;"
                   << "SELECT username FROM friends, users WHERE friends.receiver_id="
                   << user_id
                   << " AND users.id=friends.sender_id and accepted=true;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, fri_ls_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_all_friend_list(int user_id, Fri_ls_r* fri_ls_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_FRI;

    m_stringStream << "SELECT username FROM friends, users WHERE friends.sender_id="
                   << user_id
                   << " AND users.id=friends.receiver_id;"
                   << "SELECT username FROM friends, users WHERE friends.receiver_id="
                   << user_id
                   << " AND users.id=friends.sender_id;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, fri_ls_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::get_friend_invites(int user_id, Fri_ls_r* fri_ls_r){
    m_stringStream.str("");
    m_stringStream.clear();

    m_data_type = DT_FRI;

    m_stringStream << "SELECT username FROM friends, users WHERE friends.receiver_id="
                   << user_id
                   << " AND users.id=friends.sender_id and accepted=false;";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, fri_ls_r, &m_zErrMsg);

    return m_rc;
}

int DataBase::accept_friend_invite(int user_id, int friend_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "UPDATE friends SET accepted=true WHERE (receiver_id=" << user_id << " AND sender_id=" << friend_id << ")";
    m_stringStream << "  OR (sender_id=" << user_id << " AND receiver_id=" << friend_id << ");";

    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}

int DataBase::remove_friend(int user_id, int friend_id){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "DELETE FROM friends WHERE (receiver_id=" << user_id << " AND sender_id=" << friend_id << ") OR (";
    m_stringStream << "sender_id=" << user_id << " AND receiver_id=" << friend_id << ");";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}

int DataBase::create_game(Game* game_struct){
    m_stringStream.str("");
    m_stringStream.clear();

    m_stringStream << "INSERT INTO history (user_1_id, user_2_id, user_3_id, user_4_id, user_1_points, user_2_points, user_3_points, user_4_points) ";
    m_stringStream << "VALUES (" << game_struct->player1_id() << ", " << game_struct->player2_id() << ", " << game_struct->player3_id() << ", " << game_struct->player4_id();
    m_stringStream << ", " << game_struct->points_1() << ", " << game_struct->points_2() << ", " << game_struct->points_3() << ", " << game_struct->points_4() << ");";
    m_sql_request = m_stringStream.str();

    m_rc = sqlite3_exec(m_db, m_sql_request.c_str(), callback, nullptr, &m_zErrMsg);

    return m_rc;
}
