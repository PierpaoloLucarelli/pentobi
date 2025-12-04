#include <iostream>
#include <algorithm>
#include "Game.h"
#include "CpuTimeSource.h"
#include "Variant.h"
#include "Point.h"
#include "SgfNode.h"
// #include "BoardConst.h"
#include "libboardgame_mcts/SearchBase.h"
#include "libpentobi_mcts/Search.h"

using namespace libboardgame_base;
using namespace libpentobi_base;
using namespace libboardgame_base;

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

    std::cout << "Best move: " << bd.to_string(best_move) << "\n";
    Piece p = bd.get_move_piece(best_move);

    // auto* tr = bd.find_transform(best_move);
    int index = -1;
    auto pInfo = bd.get_piece_info(p);
    // auto& transforms = pInfo.get_transforms();

    // for (int i = 0; i < transforms.size(); ++i)
    //     if (transforms[i] == tr)
    //         index = i;

    // std::cout << "Best move transform: " << transforms[index] << "\n";        

    std::cout << pInfo.get_name() << std::endl;
    game.play(to_play, best_move, false);

    std::cout << "Board after best_move:\n";
    std::cout << game.get_board() << "\n";


    const auto& geo = bd.get_geometry();
    const auto& grid = bd.get_point_state();

    unsigned width  = geo.get_width();
    unsigned height = geo.get_height();

    // Initialise matrix with -1 = offboard
    std::vector<std::vector<int>> mat(height, std::vector<int>(width, -1));

    // Iterate over ALL valid points
    for (auto p : geo)
    {
        unsigned x = geo.get_x(p);
        unsigned y = geo.get_y(p);

        PointState s = grid[p];
        int val = s.is_empty() ? -1 : s.to_int();   // -1 empty, 0–3 = colours

        mat[y][x] = val;
    }

    printMatrix(mat);
    

}

