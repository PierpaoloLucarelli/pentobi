#pragma once

#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <nlohmann/json.hpp>

#include "pentobi_engine.h"

class TurnBaseSocketServer {
public:
    TurnBaseSocketServer(
        const std::string& socket_path,
        PentobiEngine engine_prototype,
        size_t max_queue_size = 64);

    ~TurnBaseSocketServer();

    void run();

private:
    // --- Server setup ---
    void setup_server();
    void start_workers();

    // --- Worker logic ---
    void worker_loop();
    void handle_client(int client_fd, PentobiEngine& engine);

    // --- Request parsing ---
    std::vector<std::vector<std::string>>
    parse_player_move_lists(
        const std::string& input,
        int& turn);

private:
    // --- Configuration ---
    std::string socket_path_;
    PentobiEngine engine_prototype_;
    size_t max_queue_size_;

    // --- Socket ---
    int server_fd_;

    // --- Thread pool ---
    std::vector<std::thread> workers_;
    std::queue<int> job_queue_;

    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    std::atomic<bool> stop_;
};

