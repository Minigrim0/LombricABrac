#include <iostream>
#include "../includes/database.hpp"

DataBase::DataBase(std::string db_path):m_is_open(false){
    m_rc = sqlite3_open(db_path.c_str(), &m_db);
    if(m_rc){
        std::cout << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        m_is_open = false;
    }
    else{
        std::cout << "Database opened" << std::endl << std::endl;
        m_is_open = true;
    }
}

DataBase::~DataBase(){
    sqlite3_close(m_db);
}

// Simple Getters
bool DataBase::is_opened() const{
    return m_is_open;
}

bool DataBase::catch_error(){
    if(m_rc != SQLITE_OK){
       std::cout << "SQL error: " << m_zErrMsg << std::endl;
       sqlite3_free(m_zErrMsg);
       return false;
    }
    std::cout << "Request successfull" << std::endl;
    return true;
}

int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}

void DataBase::get_user(std::string username) const{

}

bool DataBase::verify_user(std::string username, std::string password) const{

    return false;
}

void DataBase::register_user(std::string username, std::string password){

}
