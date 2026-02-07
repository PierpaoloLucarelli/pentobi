#include <libpentobi_base/Variant.h>
#include <libpentobi_base/PointState.h>
#include <libpentobi_base/Piece.h>
#include <libpentobi_base/Color.h>
#include "cached_moves.h"
#include <libpentobi_base/Board.h>
#include <libpentobi_mcts/Search.h>
#include <libpentobi_base/Move.h>
#include "pentobi_engine.h"
#include <libpentobi_base/Game.h>
#include <libboardgame_base/CpuTimeSource.h>
#include <iostream>

PentobiEngine::PentobiEngine(libpentobi_mcts::Float max_n_iterations,  size_t min_n_sims, double max_time): 
    max_n_iterations(max_n_iterations), min_n_sims(min_n_sims), max_time(max_time){

};

void printMatrix(const std::vector<std::vector<int>>& mat)
{
    unsigned height = mat.size();
    unsigned width  = mat[0].size();

    for (unsigned y = 0; y < height; ++y)
    {
        for (unsigned x = 0; x < width; ++x)
        {
            int v = mat[y][x];

            if (v == -1)
                std::cout << ". ";     // empty
            else
                std::cout << v << ' '; // colour ID
        }
        std::cout << '\n';
    }
}

TurnBaseMove PentobiEngine::get_best_move(
    const std::vector<std::vector<int>>& board,
    int player,
    const std::vector<std::string>& p1_moves,
    const std::vector<std::string>& p2_moves,
    const std::vector<std::string>& p3_moves,
    const std::vector<std::string>& p4_moves
) {
    std::cout << "Thinking..." << std::endl;
    BestMoveResult result;

    auto root = std::make_unique<libboardgame_base::SgfNode>();

    root->set_property("GM", "Blokus");
    root->set_property("CA", "UTF-8");

    libboardgame_base::SgfNode* last_node = nullptr;
    std::cout << "building tree..." << std::endl;
    build_sgf_4p(*root, p1_moves, p2_moves, p3_moves, p4_moves, last_node);
    std::cout << "done building tree..." << std::endl;

    libpentobi_base::Game game(libpentobi_base::Variant::classic);
    game.init(root);
    libpentobi_base::Color to_play(player);
    std::cout << "Gpoing to node..." << std::endl;
    if (last_node) {
        game.goto_node(*last_node);
    }
    game.set_to_play(to_play);
    std::cout << "Done going to node..." << std::endl;

    std::cout << "board init" << std::endl;
    std::cout << game.get_board() << std::endl;

    auto search = std::make_unique<libpentobi_mcts::Search>(
        libpentobi_base::Variant::classic,
        1, // n threads
        256 * 1024 * 1024
    );

    libpentobi_base::Move best_move;
    const libpentobi_base::Board& bd = game.get_board();

    libboardgame_base::CpuTimeSource ts;

    if(!bd.has_moves(to_play)){
      return cachedMoves[0];
    }
    bool ok = search->search(
        best_move,
        bd,
        to_play,
        max_n_iterations,
        min_n_sims,
        max_time,
        ts
    );

    game.play(to_play, best_move, false);
    
    std::cout << "Board after best_move:\n";
    std::cout << best_move.to_int() << std::endl;
    std::cout << game.get_board() << "\n";

    return cachedMoves[best_move.to_int()];
}

 

void PentobiEngine::parse_move_str(const std::string& moves_str){
    std::cout << moves_str << std::endl;
    libboardgame_base::SgfNode root;
};

 void PentobiEngine::build_sgf_4p(
    libboardgame_base::SgfNode& root,
    const std::vector<std::string>& p1,
    const std::vector<std::string>& p2,
    const std::vector<std::string>& p3,
    const std::vector<std::string>& p4,
    libboardgame_base::SgfNode*& last_node
) {
    size_t max_len = std::max(
        {p1.size(), p2.size(), p3.size(), p4.size()}
    );

    libboardgame_base::SgfNode* node = nullptr;

    for (size_t i = 0; i < max_len; ++i) {
        if (i < p1.size()) {
            node = node ? &node->create_new_child()
                        : &root.create_new_child();
            node->set_property("1", p1[i]);
        }
        if (i < p2.size()) {
            node = node ? &node->create_new_child()
                        : &root.create_new_child();
            node->set_property("2", p2[i]);
        }
        if (i < p3.size()) {
            node = node ? &node->create_new_child()
                        : &root.create_new_child();
            node->set_property("3", p3[i]);
        }
        if (i < p4.size()) {
            node = node ? &node->create_new_child()
                        : &root.create_new_child();
            node->set_property("4", p4[i]);
        }
    }

    last_node = node;
};

