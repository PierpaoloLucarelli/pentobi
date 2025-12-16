#pragma once
#include <string>


class TurnBaseSocketServer{
    public: 
        explicit TurnBaseSocketServer(const std::string& socket_path);

        ~TurnBaseSocketServer();

        void run();

    private:

        void setup_server();

        std::string socket_path_;

        int server_fd_;
};