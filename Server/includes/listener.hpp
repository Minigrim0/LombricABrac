#ifndef Listener_hpp
    #define Listener_hpp
    #include <iostream>

class Listener{
    public:
        Listener();
        ~Listener();

        void reception(int sockfd , char* str_buffer);
        void envoie_msg(int sockfd , std::string msg);
    private:

};

#endif
