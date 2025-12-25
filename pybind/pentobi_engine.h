#pragma once

#include <libpentobi_mcts/Float.h>
#include <libboardgame_base/SgfNode.h>
#include <libpentobi_base/Move.h>

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "move_convert.h"

namespace libpentobi_base {
class Game;
}

class PentobiEngine {
public:
    PentobiEngine(
        libpentobi_mcts::Float max_n_iterations,
        size_t min_n_sims,
        double max_time
    );

    TurnBaseMove get_best_move(
        int player,
        const std::vector<std::string>& p1_moves,
        const std::vector<std::string>& p2_moves,
        const std::vector<std::string>& p3_moves,
        const std::vector<std::string>& p4_moves
    );

private:
    std::unique_ptr<libpentobi_base::Game> init_game_from_history(
        const std::vector<std::string>& p1,
        const std::vector<std::string>& p2,
        const std::vector<std::string>& p3,
        const std::vector<std::string>& p4
    );

    libpentobi_base::Move run_search(
        const libpentobi_base::Game& game
    );

    void build_sgf_4p(
        libboardgame_base::SgfNode& root,
        const std::vector<std::string>& p1,
        const std::vector<std::string>& p2,
        const std::vector<std::string>& p3,
        const std::vector<std::string>& p4,
        libboardgame_base::SgfNode*& last_node
    );

    libpentobi_mcts::Float max_n_iterations;
    size_t min_n_sims;
    double max_time;

    std::unordered_map<std::string, int> pieceMap{
        {"1", 0},
        {"2", 1},
        {"I3", 3},
        {"V3", 4},
        {"I4", 5},
        {"L4", 6},
        {"T4", 7},
        {"O4", 8},
        {"Z4", 9},
        {"I5", 10},
        {"L5", 11},
        {"N", 12},
        {"P", 13},
        {"U", 14},
        {"Y", 15},
        {"T", 16},
        {"V5", 17},
        {"W", 18},
        {"Z5", 19},
        {"F", 20},
        {"X", 21}
    };
};

