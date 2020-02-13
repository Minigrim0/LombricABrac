#ifndef sql_parser_hpp
    #define sql_parser_hpp

#include <iostream>
#include "../lib/sqlite3.h"

class DataBase{
    public:
        DataBase(std::string);
        virtual ~DataBase();

        // Simple Getters
        bool is_opened() const;
        bool catch_error();
        int callback(void *data, int argc, char **argv, char **azColName);

        // Users operations
        void get_user(std::string) const;
        bool verify_user(std::string, std::string) const;

        void register_user(std::string, std::string);

        // Game history operations

        // Message operations

    private:
        sqlite3* m_db; // The database object itself

        char* m_zErrMsg;
        int m_rc;
        bool m_is_open; // Identifier to know if the databse could be opened or not
};

#endif
