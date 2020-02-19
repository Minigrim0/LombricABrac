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
        int get_user(std::string username, UserConnect* userconnect);
        int get_passwd(std::string username, std::string* password);
        int register_user(std::string username, std::string passwd);

        // Lombrics Operations
        int add_lombric(int user_id, std::string lombric_name);
        int set_lombric_name(int lombric_id, std::string username);
        int get_lombrics(int owner_id, Lomb_r* lomb_r);

        // Game history operations
        int get_history(std::string username, int index, int size, History_r* history_r);
        int add_history_entry(int player1, int player2, int player3, int player4, int player1_points, int player2_points, int player3_points, int player4_points);

        // Game rank operations
        int get_rank(int index, int size, Rank_r* rank_r);

        // Message operations
        int send_message(int sender_id, int receiver_id, std::string message);
        int get_received_messages(int user_id, Chat_r* chat_r);
        int get_sent_messages(int user_id, Chat_r* chat_r);
        int get_all_messages(int user_id, Chat_r* chat_r);

        // Friendship operations
        int add_friend(int sender_id, int receiver_id);
        int get_friend_list(int user_id, Fri_ls_r* fri_ls_r); // Get friend list only for those who accepted
        int get_all_friend_list(int user_id, Fri_ls_r* fri_ls_r); // Get every friend, even those who you didn't accept yet
        int get_friend_invites(int user_id, Fri_ls_r* fri_ls_r); // Get only the friend that are waiting to be accepted
        int accept_friend_invite(int id);
        int remove_friend(int id);

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
