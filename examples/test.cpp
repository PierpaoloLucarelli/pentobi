#include <iostream>
#include <algorithm>
#include "Game.h"
#include "Variant.h"
#include "Piece.h"
#include "BoardConst.h"

using namespace libpentobi_base;

int main() {
    Game game(Variant::classic);
    game.init();

    auto& bc  = BoardConst::get(Variant::classic);
    auto& geo = bc.get_geometry();

    // ---- SELECT PIECE ----
    Piece targetPiece;
    bc.get_piece_by_name("T5", targetPiece);

    // ---- DESIRED ABSOLUTE PLACEMENT ON BOARD ----
    // Example: a horizontal I3 at (0,0),(1,0),(2,0)
    std::vector<std::pair<int,int>> target = {
        {0, 0},
        {1, 0},
        {2, 0},
        {1,1},
        {1,2}
    };

    Move chosen = Move::null();

    // ---- SEARCH FOR THE EXACT MATCH ----
    for (Move mv(1); mv.to_int() < bc.get_range(); mv = Move(mv.to_int() + 1)) {

        if (bc.get_move_piece(mv) != targetPiece)
            continue;

        auto pts = bc.get_move_points(mv);

        std::vector<std::pair<int,int>> mvSquares;
        for (auto pt : pts)
            mvSquares.push_back({geo.get_x(pt), geo.get_y(pt)});

        // must match exact size
        if (mvSquares.size() != target.size())
            continue;

        // compare as multisets
        if (std::is_permutation(
                mvSquares.begin(), mvSquares.end(),
                target.begin()))
        {
            chosen = mv;
            break;
        }
    }

    if (chosen.is_null()) {
        std::cerr << "Couldn't find the move you requested.\n";
        return 1;
    }

    std::cout << "Chosen move id: " << chosen.to_int() << "\n";

    // ---- APPLY THE MOVE ----
    game.play(Color(0), chosen, false);

    std::cout << "Board after move:\n";
    std::cout << game.get_board() << "\n";
}