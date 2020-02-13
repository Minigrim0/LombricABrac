#ifndef Listener_hpp
    #define Listener_hpp
    #include <iostream>

class Listener{
    public:
        Listener();
        ~Listener();

        int reception(int sockfd , char** str_buffer, size_t* currrent_size_buffer);
        int envoie_msg(int sockfd , std::string msg);
    private:

};

#endif
