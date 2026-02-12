#include "server.h"
#include "logger.h"
#include "validation.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

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

//
// Constructor
//
TurnBaseSocketServer::TurnBaseSocketServer(
    const std::string& socket_path,
    PentobiEngine engine_prototype,
    size_t max_queue_size)
    : socket_path_(socket_path),
      engine_prototype_(std::move(engine_prototype)),
      max_queue_size_(max_queue_size),
      server_fd_(-1),
      stop_(false)
{
    setup_server();
    start_workers();
}

//
// Destructor
//
TurnBaseSocketServer::~TurnBaseSocketServer() {
    stop_ = true;
    queue_cv_.notify_all();

    for (auto& w : workers_) {
        if (w.joinable())
            w.join();
    }

    if (server_fd_ >= 0)
        close(server_fd_);

    unlink(socket_path_.c_str());
}

//
// Run accept loop
//
void TurnBaseSocketServer::run() {
    while (!stop_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            if (!stop_)
                Logger::instance().error("accept failed");
            continue;
        }

        std::unique_lock lock(queue_mutex_);

        queue_cv_.wait(lock, [this] {
            return stop_ || job_queue_.size() < max_queue_size_;
        });

        if (stop_) {
            close(client_fd);
            break;
        }

        job_queue_.push(client_fd);
        lock.unlock();

        queue_cv_.notify_one();
    }
}

//
// Setup UNIX socket
//
void TurnBaseSocketServer::setup_server() {
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd_ < 0)
        throw std::runtime_error("socket failed");

    fcntl(server_fd_, F_SETFD, FD_CLOEXEC);

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(),
                 sizeof(addr.sun_path) - 1);

    unlink(socket_path_.c_str());

    if (bind(server_fd_,
             reinterpret_cast<sockaddr*>(&addr),
             sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");

    if (listen(server_fd_, 16) < 0)
        throw std::runtime_error("listen failed");

    Logger::instance().info("Listening on ", socket_path_);
}

//
// Start worker threads
//
void TurnBaseSocketServer::start_workers() {
    unsigned int num_threads =
        std::thread::hardware_concurrency();

    if (num_threads == 0)
        num_threads = 4;

    Logger::instance().info("Starting ",
                            num_threads,
                            " worker threads");

    for (unsigned int i = 0; i < num_threads; ++i) {
        workers_.emplace_back(
            &TurnBaseSocketServer::worker_loop,
            this);
    }
}

void TurnBaseSocketServer::worker_loop() {
    PentobiEngine engine = engine_prototype_;

    while (true) {
        int client_fd;

        {
            std::unique_lock lock(queue_mutex_);

            queue_cv_.wait(lock, [this] {
                return stop_ || !job_queue_.empty();
            });

            if (stop_ && job_queue_.empty())
                return;

            client_fd = job_queue_.front();
            job_queue_.pop();
        }

        queue_cv_.notify_one();

        handle_client(client_fd, engine);
    }
}

void TurnBaseSocketServer::handle_client(
    int client_fd,
    PentobiEngine& engine)
{
    try {
        std::string input;
        char buffer[BUFFER_SIZE];

        while (true) {
            ssize_t n = read(client_fd, buffer, sizeof(buffer));
            if (n <= 0) break;
            input.append(buffer, n);
            if (input.find('\n') != std::string::npos) break;
        }

        if (input.empty())
            throw std::runtime_error("empty request");

        input.erase(input.find_last_not_of("\n") + 1);

        int turn;
        auto moves = parse_player_move_lists(input, turn);

        std::vector<std::vector<int>> board;

        TurnBaseMove best =
            engine.get_best_move(
                board,
                turn,
                moves[0], moves[1],
                moves[2], moves[3]);

        json out{
            {"piece", best.pieceId},
            {"row", best.row},
            {"col", best.col},
            {"rotation", best.rotation}
        };

        std::string response = out.dump() + "\n";
        write_all(client_fd,
                  response.data(),
                  response.size());
    }
    catch (const std::exception& e) {
        Logger::instance().error("Client error: ", e.what());

        json err{{"error", e.what()}};
        std::string resp = err.dump() + "\n";

        write_all(client_fd,
                  resp.data(),
                  resp.size());
    }

    close(client_fd);
}

std::vector<std::vector<std::string>>
TurnBaseSocketServer::parse_player_move_lists(
    const std::string& input,
    int& current_turn)
{
    json j = json::parse(input);

    if (!j.is_array() || j.size() != 5)
        throw std::runtime_error("Invalid protocol shape");

    std::vector<std::vector<std::string>> players(4);

    for (int i = 0; i < 4; ++i) {
        if (!j[i].is_array())
            throw std::runtime_error("Player entry not array");

        for (const auto& move : j[i]) {
            if (!move.is_string())
                throw std::runtime_error("Move not string");

            std::string m = move.get<std::string>();

            if (!is_valid_move_string(m))
                throw std::runtime_error("Invalid move: " + m);

            players[i].push_back(m);
        }
    }

    if (!j[4].is_number_integer())
        throw std::runtime_error("Turn index invalid");

    current_turn = j[4].get<int>();

    if (current_turn < 0 || current_turn > 3)
        throw std::runtime_error("Turn out of range");

    return players;
}

