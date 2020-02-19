#ifndef sql_parser_hpp
    #define sql_parser_hpp

#include <iostream>
#include "../lib/sqlite3.h"

class DataBase{
    public:
        DataBase(std::string);
        virtual ~DataBase();

        // Getters
        bool is_opened() const;
        bool catch_error();
        static int callback(void *data, int argc, char **argv, char **azColName);
        bool get_las() const;
        std::string get_last_out() const;

        // User operations
        void get_user(std::string username);
        void get_passwd(std::string username);
        bool verify_user(std::string username, std::string passwd) const;
        void register_user(std::string username, std::string passwd);

        // Lombrics Operations
        void set_lombric_name(int id, std::string uesrname);
        void get_lombrics(std::string username);

        // Game history operations
        void get_history(std::string username, int index, int size);
        void add_history_entry(int player1, int player2, int player3, int player4, int player1_points, int player2_points, int player3_points, int player4_points);

        // Game rank operations
        void get_rank(int index, int size);

        // Message operations
        void send_message(std::string sender, std::string receiver, std::string message);
        void get_messages(std::string username);

        // Friendship operations
        void add_friend(std::string sender, std::string receiver);
        void get_friend_list(std::string username); // Get friend list only for those who accepted
        void get_all_friend_list(std::string username); // Get every friend, even those who you didn't accept yet
        void get_friend_invites(std::string username); // Get only the friend that are waiting to be accepted
        void accept_friend_invite(int id);
        void remove_friend(std::string username);
        void remove_friend_invite(int id); // Remove a friend request

    private:
        sqlite3* m_db; // The database object itself

        char* m_zErrMsg;
        int m_rc;
        bool m_is_open; // Identifier to know if the databse could be opened or not
        std::string m_sql_request;
};

#endif
