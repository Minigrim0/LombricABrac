/*
    File : init_db.cpp
    Purpose : Creates the initial database and its tables
    Author : GRIMAU Florent
    Last Update : 12/02/20
*/

#include <iostream>
#include <cstring>

#include "lib/sqlite3.h"

int main(int argc, char** argv) {

    bool fill_db = false;
    if(argc == 2 && strcmp(argv[1], "true") == 0){
        fill_db = true;
    }

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
        connected      BOOL,\
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

    if(fill_db){
        sql = "INSERT INTO users (id,username,password,victory_amount) "  \
             "VALUES (1, 'Paul', 'pass', 50); " \
             "INSERT INTO users (id,username,password,victory_amount) "  \
             "VALUES (2, 'Allen', 'pass', 15); "     \
             "INSERT INTO users (id,username,password,victory_amount)" \
             "VALUES (3, 'Teddy', 'pass', 20);" \
             "INSERT INTO users (id,username,password,victory_amount)" \
             "VALUES (4, 'Mark', 'pass', 65);";

       rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

       if(rc != SQLITE_OK){
           std::cout << "SQL error: " << zErrMsg << std::endl;
           sqlite3_free(zErrMsg);
       }
       else{
           std::cout << "Table filled successfully" << std::endl;
       }
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
        finished       BOOLEAN, \
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

    if(fill_db){
        sql = "INSERT INTO history (user_1_id, user_2_id, user_3_id, user_4_id, user_1_points, user_2_points, user_3_points, user_4_points, finished) "  \
              "VALUES (1, 2, 3, 4, 50, 20, 40, 50, true);"\
              "INSERT INTO history (user_1_id, user_2_id, user_3_id, user_4_id, user_1_points, user_2_points, user_3_points, user_4_points, finished) "  \
              "VALUES (1, 2, 3, 4, 20, 10, 30, 100, true);";

       rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

       if(rc != SQLITE_OK){
           std::cout << "SQL error: " << zErrMsg << std::endl;
           sqlite3_free(zErrMsg);
       }
       else{
           std::cout << "Table filled successfully" << std::endl;
       }
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

    if(fill_db){
        sql = "INSERT INTO messages (id, sender_id, receiver_id, content) "  \
             "VALUES (1, 1, 2, 'coucou toi'); ";

       rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

       if(rc != SQLITE_OK){
           std::cout << "SQL error: " << zErrMsg << std::endl;
           sqlite3_free(zErrMsg);
       }
       else{
           std::cout << "Table filled successfully" << std::endl;
       }
    }

    std::cout << "----- Creating Friends Table ------" << std::endl << ">> ";
    sql = "CREATE TABLE friends(\
        id             INTEGER PRIMARY KEY AUTOINCREMENT,\
        sender_id      INTEGER,\
        receiver_id    INTEGER,\
        accepted       BOOL,\
        timestamp      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
        FOREIGN KEY(sender_id)   REFERENCES users(id) \
        FOREIGN KEY(receiver_id) REFERENCES users(id) \
    );";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       std::cout << "SQL error: " << zErrMsg << std::endl;
       sqlite3_free(zErrMsg);
    }
    else{
        std::cout << "Table created successfully" << std::endl;
    }

    if(fill_db){
        sql = "INSERT INTO friends (sender_id, receiver_id, accepted) "  \
              "VALUES (1, 2, true); " \
              "INSERT INTO friends (sender_id, receiver_id, accepted) "  \
              "VALUES (1, 3, false); " ;

       rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

       if(rc != SQLITE_OK){
           std::cout << "SQL error: " << zErrMsg << std::endl;
           sqlite3_free(zErrMsg);
       }
       else{
           std::cout << "Table filled successfully" << std::endl;
       }
    }

    std::cout << "----- Creating Lombrics Table -----" << std::endl << ">> ";
    sql = "CREATE TABLE worms(\
        id             INTEGER  PRIMARY KEY AUTOINCREMENT,\
        id_lomb        INTEGER,\
        name           CHAR(20) NOT NULL,\
        owner_id       INTEGER  NOT NULL\
    );";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       std::cout << "SQL error: " << zErrMsg << std::endl;
       sqlite3_free(zErrMsg);
    }
    else{
        std::cout << "Table created successfully" << std::endl;
    }

    if(fill_db){
        sql = "INSERT INTO worms (id_lomb, name, owner_id) "  \
             "VALUES (0, 'Michel', 1); " \
             "INSERT INTO worms (id_lomb, name,owner_id) "  \
             "VALUES (1, 'Patrice', 1); "     \
             "INSERT INTO worms (id_lomb, name,owner_id)" \
             "VALUES (2, 'Josh', 1);" \
             "INSERT INTO worms (id_lomb, name,owner_id)" \
             "VALUES (3, 'Fred', 1);";

       rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

       if(rc != SQLITE_OK){
           std::cout << "SQL error: " << zErrMsg << std::endl;
           sqlite3_free(zErrMsg);
       }
       else{
           std::cout << "Table filled successfully" << std::endl;
       }
    }

    sqlite3_close(db);

    return EXIT_SUCCESS;
}
