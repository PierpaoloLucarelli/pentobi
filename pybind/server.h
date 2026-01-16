#pragma once

#include <string>
#include <vector>
#include <semaphore>
#include <nlohmann/json.hpp>

#include "pentobi_engine.h"

class TurnBaseSocketServer {
public:
    TurnBaseSocketServer(const std::string& socket_path,
                         PentobiEngine engine);
    ~TurnBaseSocketServer();

    void run();

private:
    void setup_server();
    void handle_client(int client_fd);

    std::vector<std::vector<std::string>>
    parse_player_move_lists(const std::string& input, int& turn);

    std::string socket_path_;
    PentobiEngine pentobi_engine_;
    int server_fd_;

    std::counting_semaphore<32> client_limit_{32};
};

