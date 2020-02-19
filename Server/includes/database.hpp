#ifndef sql_parser_hpp
    #define sql_parser_hpp

#include <iostream>
#include <sstream>
#include "../lib/sqlite3.h"
#include "../cpl_proto/user.pb.h"


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
        UserConnect get_user(std::string username);
        std::string get_passwd(std::string username);
        bool register_user(std::string username, std::string passwd);

        // Lombrics Operations
        bool add_lombric(int user_id, std::string lombric_name);
        bool set_lombric_name(int lombric_id, std::string username);
        Lomb_r get_lombrics(int owner_id);

        // Game history operations
        History_r get_history(std::string username, int index, int size);
        bool add_history_entry(int player1, int player2, int player3, int player4, int player1_points, int player2_points, int player3_points, int player4_points);

        // Game rank operations
        Rank_r get_rank(int index, int size);

        // Message operations
        bool send_message(int sender_id, int receiver_id, std::string message);
        Chat_r get_received_messages(int user_id);
        Chat_r get_sent_messages(int user_id);
        Chat_r get_all_messages(int user_id);

        // Friendship operations
        bool add_friend(int sender_id, int receiver_id);
        Fri_ls_r get_friend_list(int user_id); // Get friend list only for those who accepted
        Fri_ls_r get_all_friend_list(int user_id); // Get every friend, even those who you didn't accept yet
        Fri_ls_r get_friend_invites(int user_id); // Get only the friend that are waiting to be accepted
        bool accept_friend_invite(int id);
        bool remove_friend(int id);

    private:
        sqlite3* m_db; // The database object itself

        char* m_zErrMsg;
        std::string* m_data;
        int m_rc;
        bool m_is_open; // Identifier to know if the databse could be opened or not
        std::string m_sql_request;
        std::ostringstream m_stringStream;
};

#endif
