#pragma once
#include <string>


class TurnBaseSocketServer{
    public: 
        explicit TurnBaseSocketServer(const std::string& socket_path);

        ~TurnBaseSocketServer();

        void run();

    private:

        void setup_server();

        void handle_client(int socket_fd);

        std::string socket_path_;

        int server_fd_;
};