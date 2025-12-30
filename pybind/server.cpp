#include "server.h"
#include "pentobi_engine.h"

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/un.h>
#include <cstring>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
const int SERVER_BUFFER_SIZE = 1024;

TurnBaseSocketServer::TurnBaseSocketServer(const std::string& socket_path, PentobiEngine engine) : socket_path_(socket_path), pentobi_engine(engine), server_fd_(-1) {
    setup_server();
};

TurnBaseSocketServer::~TurnBaseSocketServer(){
    if(server_fd_ > 0){
        close(server_fd_);
    }
    unlink(socket_path_.c_str());
}

void TurnBaseSocketServer::setup_server(){
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    std::cout << server_fd_ << std::endl;

    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    unlink(socket_path_.c_str());

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(server_fd_, 5) < 0) {
        throw std::runtime_error("Failed to listen on socket");
    }

    std::cout << "Listening on " << socket_path_ << std::endl;
};


void TurnBaseSocketServer::handle_client(int socket_fd){
    char buffer[SERVER_BUFFER_SIZE];
    ssize_t bytes = read(socket_fd, buffer, SERVER_BUFFER_SIZE - 1);
    
    if (bytes <= 0)
    {
        std::cout << "Something went wrong" << std::endl;
        return;
    }

    buffer[bytes] = '\0'; 
    std::string move_str(buffer);
    std::cout<< "Someone connected" << std::endl;
    int player_to_play;
    std::vector<std::vector<std::string>> moves = parse_player_move_lists(move_str, player_to_play);


    std::vector<std::vector<int>> board;
    TurnBaseMove best_move = pentobi_engine.get_best_move(board, player_to_play, moves[0], moves[1], moves[2], moves[3]);
    json out;
    out["piece"] = best_move.pieceId;
    out["row"] = best_move.row;
    out["col"] = best_move.col;
    out["rotation"] = best_move.rotation;
    std::string response = out.dump();
    response.push_back('\n');

    ssize_t sent = write(socket_fd, response.data(), response.size());
    if (sent < 0){
        perror("error in writing");
    }
    close(socket_fd);
};


void TurnBaseSocketServer::run(){
    while(true){
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0){
            std::cerr << "Failed to accept" << std::endl;
        } 

        std::thread(&TurnBaseSocketServer::handle_client, this, client_fd).detach();;
    }
};

std::vector<std::vector<std::string>> TurnBaseSocketServer::parse_player_move_lists(const std::string& input, int& current_turn)
{
    using json = nlohmann::json;

    std::vector<std::vector<std::string>> players(4);

    auto j = json::parse(input);

    if (!j.is_array()) {
        throw std::runtime_error("Expected top-level JSON array");
    }

    for (size_t i = 0; i < j.size() && i < 4; ++i) {
        if (!j[i].is_array()) {
            throw std::runtime_error("Each player must be an array");
        }

        for (const auto& move_str : j[i]) {
            if (!move_str.is_string()) {
                throw std::runtime_error("Move must be a string");
            }
            players[i].push_back(move_str.get<std::string>());
        }
    }

    if (!j[4].is_number_integer()) {
        throw std::runtime_error("Turn index must be an integer");
    }

    current_turn = j[4].get<int>();


    return players;
}

int main(){
    std::cout << "server starter" << std::endl;
    libpentobi_mcts::Float max_count = 100000;
    size_t min_sims = 1000;
    double max_time = 1.0; 
    PentobiEngine engine(max_count, min_sims, max_time);
    TurnBaseSocketServer server("/tmp/pentobi_server", engine);
    server.run();
    return 0;
}

// [["a20,b20,c20"],["r20,s20,t20"],[],[]]
