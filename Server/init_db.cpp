#include <iostream>

#include "lib/sqlite3.h"

int main() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("lab.db", &db);

    if(rc) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return EXIT_FAILURE;
    }
    else {
        std::cout << "Opened database successfully" << std::endl;
    }
    sqlite3_close(db);

    return EXIT_SUCCESS;
}
