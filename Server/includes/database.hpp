#ifndef sql_parser_hpp
    #define sql_parser_hpp

#include <iostream>
#include "../lib/sqlite3.h"

class DataBase{
    public:
        DataBase(std::string);
        virtual ~DataBase();

        // Users operations
        void get_user(std::string);
        void register_user(std::string, std::string);
        bool verify_user(std::string, std::string);

        // Game history operations

        // Message operations

    private:
        sqlite3* m_db;

        int rc;
        char* zErrMsg;
};

#endif
