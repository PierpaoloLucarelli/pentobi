#pragma once

#include <libpentobi_mcts/Float.h>
#include <libboardgame_base/SgfNode.h>
#include <cstddef>
#include <string>
#include <vector>

struct BestMoveResult {
    std::string piece_name;
    std::vector<std::pair<int, int>> coords;
};

class PentobiEngine{
    public:
        PentobiEngine(libpentobi_mcts::Float max_n_iterations, size_t min_n_sims, double max_time);
        void parse_move_str(const std::string& moves_str);
        void get_best_move(const std::vector<std::vector<std::string>> moves);
        BestMoveResult get_best_move(
                const std::vector<std::vector<int>>& board,
                int player,
                const std::vector<std::string>& p1_moves,
                const std::vector<std::string>& p2_moves,
                const std::vector<std::string>& p3_moves,
                const std::vector<std::string>& p4_moves
        );
    private:
        libpentobi_mcts::Float max_n_iterations;
        size_t min_n_sims;
        double max_time;

        void build_sgf_4p(
            libboardgame_base::SgfNode& root,
            const std::vector<std::string>& p1,
            const std::vector<std::string>& p2,
            const std::vector<std::string>& p3,
            const std::vector<std::string>& p4,
            libboardgame_base::SgfNode*& last_node
        );
};