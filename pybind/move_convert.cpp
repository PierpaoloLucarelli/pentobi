#include "move_convert.h"
#include <fstream>
#include "libpentobi_base/BoardConst.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/Move.h"
#include "libpentobi_base/Piece.h"
#include <utility>
#include "libpentobi_base/Game.h"
#include "libpentobi_base/Variant.h"

using namespace libpentobi_base;

std::vector<TurnBaseMove> generateMap()
{
    std::vector<TurnBaseMove> result;
    const auto& bc=libpentobi_base::BoardConst::get(libpentobi_base::Variant::classic);
    int total_moves = bc.get_range();  
    for(Move::IntType id=1;id<total_moves;++id){
        Move mv(id);
        std::cout << static_cast<double>(id)/total_moves << std::endl;
        bool found = false;
        for(int r=0;r<8;++r){
            // std::cout << "New rotation" << std::endl;
            Game game(Variant::classic);
            game.init();
            Color to_play = Color(0);
            game.play(to_play, mv, false);
            const Board& bd=game.get_board();
            Piece p=bd.get_move_piece(mv);
            auto pInfo=bd.get_piece_info(p);
            // std::cout<<bd.to_string(mv)<<std::endl;
            auto it = pieceMap.find(pInfo.get_name());
            if (it == pieceMap.end()) {
                std::cerr << "Missing key in pieceMap: "
                          << pInfo.get_name() << std::endl;
                continue; // or handle error
            }
            int pieceId = it->second;

            const Block& b=rotatedPieces[pieceId];
            const auto& coords=b.coords[r];
            std::vector<int> outBoard=makeBoard(bd);
            std::vector<int> inBoard(20*20,-1);
            auto rowCol=findMove(inBoard,outBoard,coords);
            if(rowCol){
              found = true;
              result.emplace_back(pieceId,rowCol->first,rowCol->second,r);
              break;
            }
        }
        if (!found){
          throw std::runtime_error("Not able to find move");
        }
    }
    return result;
}


std::optional<std::pair<int,int>>
findMove(const std::vector<int>& board,
         const std::vector<int>& outBoard,
         const std::vector<std::pair<int,int>>& coords)
{
    constexpr int W=20,H=20;
    for(int r=0;r<H;r++)for(int c=0;c<W;c++){
        auto tmp=board;
        bool ok=true;
        for(auto [dx,dy]:coords){
            int rr=r+dy,cc=c+dx;
            if(rr<0||rr>=H||cc<0||cc>=W){ok=false;break;}
            tmp[rr*W+cc]=0;
        }
        if(ok&&tmp==outBoard)return {{r,c}};
    }
    return std::nullopt;
}

std::vector<int> makeBoard(const Board& b){
    std::vector<int> v(20 * 20);
    const auto& geo = b.get_geometry();
    const auto& grid = b.get_point_state();
    for (auto p : geo)
    {
        unsigned x = geo.get_x(p);
        unsigned y = geo.get_y(p);

        PointState s = grid[p];
        int val = s.is_empty() ? -1 : s.to_int();   // -1 empty, 0–3 = colours

        v[y*20+x]= val;
    }
    return v;
}


int main()
{
    std::vector<TurnBaseMove> moves = generateMap();

    std::ofstream out("cached_moves.cpp");
    if (!out) {
        std::cerr << "Failed to open cached_moves.cpp\n";
        return 1;
    }

    out << "// GENERATED FILE — DO NOT EDIT\n";
    out << "#include <vector>\n";
    out << "#include \"move_convert.h\"\n\n";

    out << "const std::vector<TurnBaseMove> cachedMoves = {\n";

    for (const auto& m : moves) {
        out << "    {"
            << static_cast<int>(m.pieceId) << ", "
            << static_cast<int>(m.row) << ", "
            << static_cast<int>(m.col) << ", "
            << static_cast<int>(m.rotation)
            << "},\n";
    }

    out << "};\n";
    out.close();

    std::cout << "Generated cached_moves.cpp with "
              << moves.size() << " entries\n";

    return 0;
}
