#ifndef sql_parser_hpp
    #define sql_parser_hpp

#include <iostream>
#include <sstream>
#include "../lib/sqlite3.h"
#include "../lib/bcrypt.h"
#include "../cpl_proto/user.pb.h"

#define DT_USR 1  // Datatype UserConnect
#define DT_STR 2  // Datatype String
#define DT_INT 3  // Datatype Int
#define DT_LOM 4  // Datatype Lomb_r
#define DT_HIS 5  // Datatype History_r
#define DT_RAN 6  // Datatype Rank_r
#define DT_CHA 7  // Datatype Chat_r
#define DT_FRI 8  // Datatype Fri_ls_r
#define DT_RID 9  // Datatype Fri_ls_r
#define DT_END 10 // Datatype Fri_ls_r

class DataBase{
    public:
        DataBase(std::string);
        virtual ~DataBase();

        // Getters
        bool is_opened() const;
        bool catch_error();
        static int callback(void *data, int argc, char **argv, char **azColName);
        bool get_las() const;

        // User operations
        int get_user(std::string username, UserConnect* userconnect);
        int get_user(int user_id, UserConnect* userconnect);
        int get_user_id(std::string username, int* id);
        int get_passwd(std::string username, std::string* password);
        int register_user(std::string username, std::string passwd);
        int connect_user(bool connected, std::string username);

        // Lombrics Operations
        int add_lombric(int user_id, int lombric_id, std::string lombric_name);
        int set_lombric_name(int lombric_id, int user_id, std::string username);
        int get_lombrics(int owner_id, Lomb_r* lomb_r);
        int get_x_lombrics(int owner_id, int nbLombs, End_tour* lomb_r);

        // Game history operations
        int get_history(int user_id, int index, int size, History_r* history_r);
        int add_history_entry(int player1, int player2, int player3, int player4, int player1_points, int player2_points, int player3_points, int player4_points);

        // Game rank operations
        int get_rank(int index, int size, Rank_r* rank_r);

        // Message operations
        int send_message(int sender_id, int receiver_id, std::string message);
        int get_received_messages(int user_id, Chat_r* chat_r);
        int get_sent_messages(int user_id, Chat_r* chat_r);
        int get_all_messages(int user_id, Chat_r* chat_r);
        int get_messages_sent_to(int sender_id, int receiver_id, Chat_r* chat_r);
        int get_messages_received_from(int receiver_id, int sender_id, Chat_r* chat_r);
        int get_convo(int user1_id, int user2_id, Chat_r* chat_r);


        // Friendship operations
        int add_friend(int sender_id, int receiver_id);
        int get_friend_list(int user_id, Fri_ls_r* fri_ls_r); // Get friend list only for those who accepted
        int get_all_friend_list(int user_id, Fri_ls_r* fri_ls_r); // Get every friend, even those who you didn't accept yet
        int get_friend_invites(int user_id, Fri_ls_r* fri_ls_r); // Get only the friend that are waiting to be accepted
        int accept_friend_invite(int user_id, int friend_id);
        int remove_friend(int user_id, int friend_id);
        int is_online(int friend_id, int* online);

        // Create game
        int create_room(int owner_id);
        int get_last_room_id(int *room_id);
        int get_room_id_from_owner_id(int owner_id, int* room_id);

    private:
        static uint8_t m_data_type; // The type of data the callback has to deal with

        int m_rc; // The return value of the last sqlite3_exec() call
        bool m_is_open; // Identifier to define wether the db is opened or not

        sqlite3* m_db; // The database object itself

        char* m_zErrMsg; // The error message in case there a problem
        std::string m_sql_request; // The last performed request
        std::ostringstream m_stringStream; // Object used to properly concatenate requests

        char salt[BCRYPT_HASHSIZE]; //salt for hash function
        char hash[BCRYPT_HASHSIZE]; // store the hash of passw
};

#endif
