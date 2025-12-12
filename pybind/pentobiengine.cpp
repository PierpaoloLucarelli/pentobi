#include "Game.h"
#include "CpuTimeSource.h"
#include "Variant.h"
#include "SgfNode.h"
#include "libpentobi_mcts/Search.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

using namespace libpentobi_base;
using namespace libboardgame_base;

static void print_help(const char* prog) {
    std::cout <<
        "Pentobi Engine – 4-player Blokus move generator\n\n"
        "USAGE:\n"
        "  " << prog << " [OPTIONS]\n\n"
        "OPTIONS:\n"
        "  --player N\n"
        "      Player to move (1–4). Default: 1\n\n"
        "  --p1 MOVE [MOVE ...]\n"
        "      Moves played by player 1, in order.\n"
        "      Each MOVE is a comma-separated list of coordinates.\n\n"
        "  --p2 MOVE [MOVE ...]\n"
        "      Moves played by player 2.\n\n"
        "  --p3 MOVE [MOVE ...]\n"
        "      Moves played by player 3.\n\n"
        "  --p4 MOVE [MOVE ...]\n"
        "      Moves played by player 4.\n\n"
        "  --help\n"
        "      Show this help message and exit.\n\n"
        "EXAMPLE:\n"
        "  " << prog << " \\\n"
        "    --player 3 \\\n"
        "    --p1 a20,b20,c20 d19,e19 \\\n"
        "    --p2 t20,s20,r20 q19,p19 \\\n"
        "    --p3 a1,b1,c1 \\\n"
        "    --p4 t1,s1,r1\n";
}


struct BestMoveResult {
    std::string piece_name;
    std::vector<std::pair<int, int>> coords;
};

static void build_sgf_4p(
    SgfNode& root,
    const std::vector<std::string>& p1,
    const std::vector<std::string>& p2,
    const std::vector<std::string>& p3,
    const std::vector<std::string>& p4,
    SgfNode*& last_node
) {
    size_t max_len = std::max(
        {p1.size(), p2.size(), p3.size(), p4.size()}
    );

    SgfNode* node = nullptr;

    for (size_t i = 0; i < max_len; ++i) {
        if (i < p1.size()) {
            node = node ? &node->create_new_child()
                        : &root.create_new_child();
            node->set_property("1", p1[i]);
        }
        if (i < p2.size()) {
            node = &node->create_new_child();
            node->set_property("2", p2[i]);
        }
        if (i < p3.size()) {
            node = &node->create_new_child();
            node->set_property("3", p3[i]);
        }
        if (i < p4.size()) {
            node = &node->create_new_child();
            node->set_property("4", p4[i]);
        }
    }

    last_node = node;
}

BestMoveResult get_best_move(
    const std::vector<std::vector<int>>& board,
    int player,
    const std::vector<std::string>& p1_moves,
    const std::vector<std::string>& p2_moves,
    const std::vector<std::string>& p3_moves,
    const std::vector<std::string>& p4_moves
) {
    BestMoveResult result;

    auto root = std::make_unique<SgfNode>();

    root->set_property("GM", "Blokus");
    root->set_property("CA", "UTF-8");

    SgfNode* last_node = nullptr;
    build_sgf_4p(*root, p1_moves, p2_moves, p3_moves, p4_moves, last_node);

    Game game(Variant::classic);
    game.init(root);

    if (last_node) {
        game.goto_node(*last_node);
    }

    std::cout << "Board init:\n";
    std::cout << game.get_board() << "\n";

    auto search = std::make_unique<libpentobi_mcts::Search>(
        Variant::classic,
        1,
        256 * 1024 * 1024
    );

    Move best_move;
    const Board& bd = game.get_board();
    Color to_play = bd.get_effective_to_play();

    CpuTimeSource ts;

    libpentobi_mcts::Float max_count = 100000;       // max number of simulations
    size_t min_sims = 1000;         // minimum number of simulations
    double max_time = 1.0;          // 1 second
    bool ok = search->search(
        best_move,
        bd,
        to_play,
        max_count,
        min_sims,
        max_time,
        ts
    );

    Piece p = bd.get_move_piece(best_move);
    auto pInfo = bd.get_piece_info(p);
    result.piece_name = pInfo.get_name();
    const auto& geo = bd.get_geometry();

    for (libpentobi_base::Point p : bd.get_move_points(best_move)) {
        unsigned x = geo.get_x(p);
        unsigned y = geo.get_y(p);
        result.coords.emplace_back(x, y);
    }

    return result;
}

static std::vector<std::string> collect_moves(
    int& i,
    int argc,
    char** argv
) {
    std::vector<std::string> moves;
    while (i + 1 < argc && argv[i + 1][0] != '-') {
        moves.emplace_back(argv[++i]);
    }
    return moves;
}


int main(int argc, char** argv) {

    if (argc == 1) {
    print_help(argv[0]);
    return 0;
    }

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help") {
            print_help(argv[0]);
            return 0;
        }
    }

    int player = 1;

    std::vector<std::string> p1_moves;
    std::vector<std::string> p2_moves;
    std::vector<std::string> p3_moves;
    std::vector<std::string> p4_moves;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--player" && i + 1 < argc) {
            player = std::stoi(argv[++i]);
        }
        else if (arg == "--p1") {
            p1_moves = collect_moves(i, argc, argv);
        }
        else if (arg == "--p2") {
            p2_moves = collect_moves(i, argc, argv);
        }
        else if (arg == "--p3") {
            p3_moves = collect_moves(i, argc, argv);
        }
        else if (arg == "--p4") {
            p4_moves = collect_moves(i, argc, argv);
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    // Dummy board for now (Pentobi ignores it because SGF drives state)
    std::vector<std::vector<int>> board;

    BestMoveResult res = get_best_move(
        board,
        player,
        p1_moves,
        p2_moves,
        p3_moves,
        p4_moves
    );

    std::cout << "{\n";
    std::cout << "  \"piece\": \"" << res.piece_name << "\",\n";
    std::cout << "  \"coords\": [";

    for (size_t i = 0; i < res.coords.size(); ++i) {
        const auto& [x, y] = res.coords[i];
        std::cout << "[" << x << "," << y << "]";
        if (i + 1 < res.coords.size())
            std::cout << ",";
    }

    std::cout << "]\n}\n";

    return 0;
}
