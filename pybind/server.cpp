#include "server.h"
#include "logger.h"
#include "validation.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <fcntl.h>

using json = nlohmann::json;
constexpr size_t BUFFER_SIZE = 1024;

static void write_all(int fd, const char* data, size_t size) {
    size_t written = 0;
    while (written < size) {
        ssize_t n = write(fd, data + written, size - written);
        if (n <= 0) {
            throw std::runtime_error("write failed");
        }
        written += n;
    }
}

TurnBaseSocketServer::TurnBaseSocketServer(
    const std::string& socket_path,
    PentobiEngine engine)
    : socket_path_(socket_path),
      pentobi_engine_(std::move(engine)),
      server_fd_(-1)
{
    setup_server();
}

TurnBaseSocketServer::~TurnBaseSocketServer() {
    if (server_fd_ >= 0) {
        close(server_fd_);
    }
    unlink(socket_path_.c_str());
}

void TurnBaseSocketServer::setup_server() {
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("socket failed");
    }

    fcntl(server_fd_, F_SETFD, FD_CLOEXEC);

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(),
                 sizeof(addr.sun_path) - 1);

    unlink(socket_path_.c_str());

    if (bind(server_fd_,
             reinterpret_cast<sockaddr*>(&addr),
             sizeof(addr)) < 0) {
        throw std::runtime_error("bind failed");
    }

    if (listen(server_fd_, 16) < 0) {
        throw std::runtime_error("listen failed");
    }

    Logger::instance().info("Listening on ", socket_path_);
}

void TurnBaseSocketServer::run() {
    while (true) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            Logger::instance().error("accept failed");
            continue;
        }

        client_limit_.acquire();
        std::thread([this, client_fd] {
            handle_client(client_fd);
            client_limit_.release();
        }).detach();
    }
}

void TurnBaseSocketServer::handle_client(int client_fd) {
    try {
        std::string input;
        char buffer[BUFFER_SIZE];

        while (true) {
            ssize_t n = read(client_fd, buffer, sizeof(buffer));
            if (n <= 0) break;
            input.append(buffer, n);
            if (input.find('\n') != std::string::npos) break;
        }

        if (input.empty()) {
            throw std::runtime_error("empty request");
        }

        input.erase(input.find_last_not_of("\n") + 1);

        int turn;
        auto moves = parse_player_move_lists(input, turn);

        std::vector<std::vector<int>> board;
        TurnBaseMove best =
            pentobi_engine_.get_best_move(
                board, turn,
                moves[0], moves[1], moves[2], moves[3]);

        json out{
            {"piece", best.pieceId},
            {"row", best.row},
            {"col", best.col},
            {"rotation", best.rotation}
        };

        std::string response = out.dump() + "\n";
        write_all(client_fd, response.data(), response.size());
    }
    catch (const std::exception& e) {
        Logger::instance().error("Client error: ", e.what());
        json err{{"error", e.what()}};
        std::string resp = err.dump() + "\n";
        write_all(client_fd, resp.data(), resp.size());
    }

    close(client_fd);
}

std::vector<std::vector<std::string>>
TurnBaseSocketServer::parse_player_move_lists(
    const std::string& input,
    int& current_turn)
{
    json j = json::parse(input);

    if (!j.is_array() || j.size() != 5) {
        throw std::runtime_error("Invalid protocol shape");
    }

    std::vector<std::vector<std::string>> players(4);

    for (int i = 0; i < 4; ++i) {
        if (!j[i].is_array()) {
            throw std::runtime_error("Player entry not array");
        }

        for (const auto& move : j[i]) {
            if (!move.is_string()) {
                throw std::runtime_error("Move not string");
            }

            std::string m = move.get<std::string>();
            if (!is_valid_move_string(m)) {
                throw std::runtime_error("Invalid move: " + m);
            }

            players[i].push_back(m);
        }
    }

    if (!j[4].is_number_integer()) {
        throw std::runtime_error("Turn index invalid");
    }

    current_turn = j[4].get<int>();
    if (current_turn < 0 || current_turn > 3) {
        throw std::runtime_error("Turn out of range");
    }

    return players;
}

