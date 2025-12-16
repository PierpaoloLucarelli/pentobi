#include "server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/un.h>
#include <cstring>


TurnBaseSocketServer::TurnBaseSocketServer(const std::string& socket_path) : socket_path_(socket_path), server_fd_(-1) {
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

void TurnBaseSocketServer::run(){
    while(true){
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0){
            std::cerr << "Failed to accept" << std::endl;
        } 

        std::cout<< "Someone connected" << std::endl;
        close(client_fd);
    }
};


int main(){
    TurnBaseSocketServer server("/tmp/pentobi_server");

    server.run();
    return 0;
}