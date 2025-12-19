#pragma once

#include <string>
#include "pentobi_engine.h"


class TurnBaseSocketServer{
    public: 
        explicit TurnBaseSocketServer(const std::string& socket_path, PentobiEngine engine);

        ~TurnBaseSocketServer();

        void run();

    private:

        void setup_server();

        void handle_client(int);

        std::string socket_path_;

        int server_fd_;

        PentobiEngine pentobi_engine;
};