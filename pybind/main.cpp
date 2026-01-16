#include "server.h"
#include "crash_handler.h"
#include "logger.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        Logger::instance().error("Usage: server <socket_path>");
        return 1;
    }

    install_crash_handlers();

    libpentobi_mcts::Float max_count = 1000;
    size_t min_sims = 1000;
    double max_time = 1.0;

    PentobiEngine engine(max_count, min_sims, max_time);

    TurnBaseSocketServer server(argv[1], engine);
    server.run();
}

