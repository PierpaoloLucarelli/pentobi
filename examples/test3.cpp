
#include <iostream>
#include <algorithm>
#include "Game.h"
#include "CpuTimeSource.h"
#include "Variant.h"
#include "Point.h"
#include "SgfNode.h"
#include "BoardConst.h"
#include "libboardgame_mcts/SearchBase.h"
#include "libpentobi_mcts/Search.h"

using namespace libboardgame_base;
using namespace libpentobi_base;
using namespace libboardgame_base;


int main() {
    auto root = std::make_unique<SgfNode>();
    auto search = std::make_unique<libpentobi_mcts::Search>(
        Variant::classic,
        1,
        256 * 1024 * 1024
    );

    Move best_move;

    root->set_property("GM", "Blokus");
    root->set_property("CA", "UTF-8");

    SgfNode& moveNode = root->create_new_child();
    moveNode.set_property("1", "a20,b20,c20");

    Game game(Variant::classic);
    game.init(root);

    game.goto_node(moveNode);

    std::cout << "Board init:\n";
    std::cout << game.get_board() << "\n";

    const Board& bd = game.get_board();
    Color to_play = bd.get_effective_to_play();
    const BoardConst& bc = BoardConst::get(Variant::classic);
    std::cout << bc.get_range() << std::endl;
//    for (Move::IntType id = 0; id < bc.get_range(); ++id)
    //{
    //    Move mv(id);
    //    std::cout << bd.to_string(mv) << std::endl;
    //} 

}

