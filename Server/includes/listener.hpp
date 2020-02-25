#ifndef Listener_hpp
    #define Listener_hpp
    #include <iostream>

class Listener{
    public:
        Listener(int socket_client);
        ~Listener();

        uint8_t reception_type();
        int reception();
        int envoie_bool(int type_msg ,int boolint);
        int envoie_msg(uint8_t type_msg , std::string msg);
        char* get_buffer();

    private:
        char* m_str_parser;
        int m_received_size;
        int m_res;
        uint32_t m_len_char;
        uint32_t m_packet_size;
        uint32_t m_sent_size;
        char *str_buffer;
        size_t current_size_buffer;
        int sockfd;
};

#endif
