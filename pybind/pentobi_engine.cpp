#include "pentobi_engine.h"
#include "move_convert.h"

#include <libpentobi_base/Variant.h>
#include <libpentobi_base/PointState.h>
#include <libpentobi_base/Piece.h>
#include <libpentobi_base/Color.h>
#include <libpentobi_base/Board.h>
#include <libpentobi_base/Game.h>
#include <libpentobi_mcts/Search.h>
#include <libpentobi_base/Move.h>
#include <libboardgame_base/CpuTimeSource.h>

#include <vector>
#include <memory>
#include <algorithm>

static constexpr int BOARD_W = 20;
static constexpr int BOARD_H = 20;

PentobiEngine::PentobiEngine(
    libpentobi_mcts::Float max_n_iterations,
    size_t min_n_sims,
    double max_time
)
    : max_n_iterations(max_n_iterations),
      min_n_sims(min_n_sims),
      max_time(max_time)
{
}


static std::vector<int>
board_to_1d(const libpentobi_base::Board& bd)
{
    std::vector<int> out(BOARD_W * BOARD_H, -1);
    const auto& geo  = bd.get_geometry();
    const auto& grid = bd.get_point_state();

    for (auto p : geo) {
        unsigned x = geo.get_x(p);
        unsigned y = geo.get_y(p);
        auto s = grid[p];
        out[y * BOARD_W + x] = s.is_empty() ? -1 : s.to_int();
    }

    return out;
}

TurnBaseMove PentobiEngine::get_best_move(
    int player,
    const std::vector<std::string>& p1_moves,
    const std::vector<std::string>& p2_moves,
    const std::vector<std::string>& p3_moves,
    const std::vector<std::string>& p4_moves
) {

    auto root = std::make_unique<libboardgame_base::SgfNode>();
    root->set_property("GM", "Blokus");
    root->set_property("CA", "UTF-8");

    libboardgame_base::SgfNode* last_node = nullptr;
    build_sgf_4p(*root, p1_moves, p2_moves, p3_moves, p4_moves, last_node);

    libpentobi_base::Game game(libpentobi_base::Variant::classic);
    game.init(root);

    if (last_node) {
        game.goto_node(*last_node);
    }

    auto search = std::make_unique<libpentobi_mcts::Search>(
        libpentobi_base::Variant::classic,
        1,
        256 * 1024 * 1024
    );

    libpentobi_base::Move best;
    libboardgame_base::CpuTimeSource ts;

    const auto& bdin = game.get_board();
    std::cout << bdin << std::endl;
    search->search(
        best,
        bdin,
        bdin.get_effective_to_play(),
        max_n_iterations,
        min_n_sims,
        max_time,
        ts
    );

    std::cout << "end" << std::endl;
    libpentobi_base::Color to_play =
        game.get_board().get_effective_to_play();

    std::vector<int> boardIn = board_to_1d(bdin);
    game.play(to_play, best, false);

    const auto& bd = game.get_board();
    libpentobi_base::Piece piece = bd.get_move_piece(best);
    auto info = bd.get_piece_info(piece);

    int piece_id = pieceMap[info.get_name()];

    std::vector<int> boardOut = board_to_1d(bd);

    return convertMove(piece_id, player, boardIn, boardOut);
}

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

