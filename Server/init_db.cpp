/*
    File : init_db.cpp
    Purpose : Creates the initial database and its tables
    Author : GRIMAU Florent
    Last Update : 12/02/20
*/

#include <iostream>
#include <cstring>

#include "lib/sqlite3.h"

int main() {
    sqlite3 *db; // The database object
    std::string sql; // String that will contain the SQL query

    int rc;
    char* zErrMsg = nullptr;

    // Opening the database (Creating it if the file does not exist)
    rc = sqlite3_open("lab.db", &db);

    if(rc){
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Database opened" << std::endl << std::endl;

    std::cout << "------- Creating Users Table -------" << std::endl << ">> ";
    sql = "CREATE TABLE users(\
        id             INTEGER  PRIMARY KEY AUTOINCREMENT,\
        username       CHAR(40) NOT NULL,\
        password       CHAR(40) NOT NULL,\
        victory_amount INT\
    );";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       std::cout << "SQL error: " << zErrMsg << std::endl;
       sqlite3_free(zErrMsg);
    }
    else{
        std::cout << "Table created successfully" << std::endl;
    }

    std::cout << "------ Creating History Table ------" << std::endl << ">> ";
    sql = "CREATE TABLE history(\
        id             INTEGER PRIMARY KEY AUTOINCREMENT,\
        user_1_id      INTEGER,\
        user_2_id      INTEGER,\
        user_3_id      INTEGER,\
        user_4_id      INTEGER,\
        user_1_points  SMALLINT,\
        user_2_points  SMALLINT,\
        user_3_points  SMALLINT,\
        user_4_points  SMALLINT,\
        timestamp      TIMESTAMP DEFAULT CURRENT_TIMESTAMP\
    );";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       std::cout << "SQL error: " << zErrMsg << std::endl;
       sqlite3_free(zErrMsg);
    }
    else{
        std::cout << "Table created successfully" << std::endl;
    }

    std::cout << "----- Creating Messages Table ------" << std::endl << ">> ";
    sql = "CREATE TABLE messages(\
        id             INTEGER PRIMARY KEY AUTOINCREMENT,\
        sender_id      INTEGER,\
        receiver_id    INTEGER,\
        content        TEXT,\
        timestamp      TIMESTAMP DEFAULT CURRENT_TIMESTAMP\
    );";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       std::cout << "SQL error: " << zErrMsg << std::endl;
       sqlite3_free(zErrMsg);
    }
    else{
        std::cout << "Table created successfully" << std::endl;
    }

    sqlite3_close(db);

    return EXIT_SUCCESS;
}
