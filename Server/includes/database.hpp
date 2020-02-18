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

        void get_user(std::string username);
        void get_passwd(std::string username);
        bool verify_user(std::string username, std::string passwd) const;

        void register_user(std::string username, std::string passwd);

        // Game history operations

        // Message operations

    private:
        sqlite3* m_db; // The database object itself

        char* m_zErrMsg;
        int m_rc;
        bool m_is_open; // Identifier to know if the databse could be opened or not
        std::string m_sql_request;
};

#endif
