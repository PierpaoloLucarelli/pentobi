
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
    Game game(Variant::classic);
    game.init();

    std::cout << "Board init:\n";
    
    std::cout << game.get_board() << "\n";
    Move mv = Move(144); 
    Color to_play = Color(0);
    game.play(to_play, mv, false);

    std::cout << "Board after best_move:\n";
    std::cout << game.get_board() << "\n";
}
