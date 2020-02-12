#include <iostream>
#include <cstring>

#include "lib/sqlite3.h"

int main() {
    sqlite3 *db;
    int rc;
    char *zErrMsg = nullptr;
    std::string sql;

    rc = sqlite3_open("lab.db", &db);

    if(rc) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return EXIT_FAILURE;
    }
    else {
        std::cout << "Opened database successfully" << std::endl;
    }

    /* Create SQL statement */
    sql = "CREATE TABLE users(\
        id             INTEGER  PRIMARY KEY AUTOINCREMENT,\
        username       CHAR(40) NOT NULL,\
        password       CHAR(40) NOT NULL,\
        victory_amount INT\
    );";

    std::cout << "Creating Users Table" << std::endl;

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    else {
       fprintf(stdout, "Table created successfully\n");
    }

    /* Create SQL statement */
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

    std::cout << "Creating History Table" << std::endl;

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    else {
       fprintf(stdout, "Table created successfully\n");
    }

    sqlite3_close(db);

    return EXIT_SUCCESS;
}
