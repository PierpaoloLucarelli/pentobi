#pragma once

#include <string>
#include "pentobi_engine.h"
#include <vector>

class TurnBaseSocketServer{
    public: 
        explicit TurnBaseSocketServer(const std::string& socket_path, PentobiEngine engine);

        ~TurnBaseSocketServer();

        void run();

    private:

        void setup_server();

        void handle_client(int);

        std::vector<std::vector<std::string>> parse_player_move_lists(const std::string& input);

        std::string socket_path_;

        int server_fd_;

        PentobiEngine pentobi_engine;
};