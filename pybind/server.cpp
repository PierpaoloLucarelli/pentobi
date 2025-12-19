#include "server.h"
#include "pentobi_engine.h"

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/un.h>
#include <cstring>
#include <thread>


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

void simulateLongProcess(int steps = 10, int msPerStep = 500)
{
    for (int i = 1; i <= steps; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(msPerStep));
        std::cout << "Progress: " << (i * 100 / steps) << "%\n";
    }
}

void TurnBaseSocketServer::handle_client(int socket_fd){
    simulateLongProcess();
    std::cout<< "Someone connected" << std::endl;
    std::string s = "movestr";
    pentobi_engine.parse_move_str(s);
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


int main(){
    libpentobi_mcts::Float max_count = 100000;
    size_t min_sims = 1000;
    double max_time = 1.0; 
    PentobiEngine engine(max_count, min_sims, max_time);
    TurnBaseSocketServer server("/tmp/pentobi_server", engine);
    server.run();
    return 0;
}