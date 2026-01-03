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

bool sameBoard(const std::vector<int>& a, const std::vector<int>& b) {

    return a == b;
}

void printBoard(libpentobi_base::Board& bd){
  std::cout << bd << std::endl;
}

const Block rotatedPieces[21] = {
	Block('a', 1, 1, 1, {{{0, 0}},{{0, 0}},{{0, 0}},{{0, 0}},{{0, 0}},{{0, 0}},{{0, 0}},{{0, 0}}}, {0}),
	Block('b', 2, 1, 2, {{{0, 0},{0, 1}},{{0, 0},{-1, 0}},{{0, 0},{0, -1}},{{0, 0},{1, 0}},{{0, 0},{0, 1}},{{0, 0},{-1, 0}},{{0, 0},{0, -1}},{{0, 0},{1, 0}}}, {0,1}),
	Block('c', 3, 1, 3, {{{0, 0},{0, 1},{0, -1}},{{0, 0},{-1, 0},{1, 0}},{{0, 0},{0, -1},{0, 1}},{{0, 0},{1, 0},{-1, 0}},{{0, 0},{0, 1},{0, -1}},{{0, 0},{-1, 0},{1, 0}},{{0, 0},{0, -1},{0, 1}},{{0, 0},{1, 0},{-1, 0}}}, {1,2}),
	Block('d', 3, 2, 2, {{{0, 0},{1, 0},{0, -1}},{{0, 0},{0, 1},{1, 0}},{{0, 0},{-1, 0},{0, 1}},{{0, 0},{0, -1},{-1, 0}},{{0, 0},{-1, 0},{0, -1}},{{0, 0},{0, -1},{1, 0}},{{0, 0},{1, 0},{0, 1}},{{0, 0},{0, 1},{-1, 0}}}, {0,1,2}),
	Block('e', 4, 1, 4, {{{0, 0},{0, 1},{0, 2},{0, -1}},{{0, 0},{-1, 0},{-2, 0},{1, 0}},{{0, 0},{0, -1},{0, -2},{0, 1}},{{0, 0},{1, 0},{2, 0},{-1, 0}},{{0, 0},{0, 1},{0, 2},{0, -1}},{{0, 0},{-1, 0},{-2, 0},{1, 0}},{{0, 0},{0, -1},{0, -2},{0, 1}},{{0, 0},{1, 0},{2, 0},{-1, 0}}}, {2,3}),
	Block('f', 4, 2, 3, {{{0, 0},{0, -1},{0, 1},{-1, 1}},{{0, 0},{1, 0},{-1, 0},{-1, -1}},{{0, 0},{0, 1},{0, -1},{1, -1}},{{0, 0},{-1, 0},{1, 0},{1, 1}},{{0, 0},{0, -1},{0, 1},{1, 1}},{{0, 0},{1, 0},{-1, 0},{-1, 1}},{{0, 0},{0, 1},{0, -1},{-1, -1}},{{0, 0},{-1, 0},{1, 0},{1, -1}}}, {1,2,3}),
	Block('g', 4, 2, 3, {{{0, 0},{1, 0},{0, 1},{0, -1}},{{0, 0},{0, 1},{-1, 0},{1, 0}},{{0, 0},{-1, 0},{0, -1},{0, 1}},{{0, 0},{0, -1},{1, 0},{-1, 0}},{{0, 0},{-1, 0},{0, 1},{0, -1}},{{0, 0},{0, -1},{-1, 0},{1, 0}},{{0, 0},{1, 0},{0, -1},{0, 1}},{{0, 0},{0, 1},{1, 0},{-1, 0}}}, {1,2,3}),
	Block('h', 4, 2, 2, {{{0, 0},{1, 0},{0, 1},{1, 1}},{{0, 0},{0, 1},{-1, 0},{-1, 1}},{{0, 0},{-1, 0},{0, -1},{-1, -1}},{{0, 0},{0, -1},{1, 0},{1, -1}},{{0, 0},{-1, 0},{0, 1},{-1, 1}},{{0, 0},{0, -1},{-1, 0},{-1, -1}},{{0, 0},{1, 0},{0, -1},{1, -1}},{{0, 0},{0, 1},{1, 0},{1, 1}}}, {0,1,2,3}),
	Block('i', 4, 3, 2, {{{-1, 0},{0, 0},{0, 1},{1, 1}},{{0, -1},{0, 0},{-1, 0},{-1, 1}},{{1, 0},{0, 0},{0, -1},{-1, -1}},{{0, 1},{0, 0},{1, 0},{1, -1}},{{1, 0},{0, 0},{0, 1},{-1, 1}},{{0, 1},{0, 0},{-1, 0},{-1, -1}},{{-1, 0},{0, 0},{0, -1},{1, -1}},{{0, -1},{0, 0},{1, 0},{1, 1}}}, {0,1,2,3}),
	Block('j', 5, 1, 5, {{{0, 0},{0, 1},{0, 2},{0, -1},{0, -2}},{{0, 0},{-1, 0},{-2, 0},{1, 0},{2, 0}},{{0, 0},{0, -1},{0, -2},{0, 1},{0, 2}},{{0, 0},{1, 0},{2, 0},{-1, 0},{-2, 0}},{{0, 0},{0, 1},{0, 2},{0, -1},{0, -2}},{{0, 0},{-1, 0},{-2, 0},{1, 0},{2, 0}},{{0, 0},{0, -1},{0, -2},{0, 1},{0, 2}},{{0, 0},{1, 0},{2, 0},{-1, 0},{-2, 0}}}, {2,4}),
	Block('k', 5, 2, 4, {{{0, 0},{0, 1},{0, -2},{0, -1},{-1, 1}},{{0, 0},{-1, 0},{2, 0},{1, 0},{-1, -1}},{{0, 0},{0, -1},{0, 2},{0, 1},{1, -1}},{{0, 0},{1, 0},{-2, 0},{-1, 0},{1, 1}},{{0, 0},{0, 1},{0, -2},{0, -1},{1, 1}},{{0, 0},{-1, 0},{2, 0},{1, 0},{-1, 1}},{{0, 0},{0, -1},{0, 2},{0, 1},{-1, -1}},{{0, 0},{1, 0},{-2, 0},{-1, 0},{1, -1}}}, {1,2,4}),
	Block('l', 5, 2, 4, {{{0, -2},{0, -1},{0, 0},{-1, 0},{-1, 1}},{{2, 0},{1, 0},{0, 0},{0, -1},{-1, -1}},{{0, 2},{0, 1},{0, 0},{1, 0},{1, -1}},{{-2, 0},{-1, 0},{0, 0},{0, 1},{1, 1}},{{0, -2},{0, -1},{0, 0},{1, 0},{1, 1}},{{2, 0},{1, 0},{0, 0},{0, 1},{-1, 1}},{{0, 2},{0, 1},{0, 0},{-1, 0},{-1, -1}},{{-2, 0},{-1, 0},{0, 0},{0, -1},{1, -1}}}, {0,2,3,4}),
	Block('m', 5, 2, 3, {{{0, -1},{-1, 0},{0, 0},{-1, 1},{0, 1}},{{1, 0},{0, -1},{0, 0},{-1, -1},{-1, 0}},{{0, 1},{1, 0},{0, 0},{1, -1},{0, -1}},{{-1, 0},{0, 1},{0, 0},{1, 1},{1, 0}},{{0, -1},{1, 0},{0, 0},{1, 1},{0, 1}},{{1, 0},{0, 1},{0, 0},{-1, 1},{-1, 0}},{{0, 1},{-1, 0},{0, 0},{-1, -1},{0, -1}},{{-1, 0},{0, -1},{0, 0},{1, -1},{1, 0}}}, {0,1,3,4}),
	Block('n', 5, 2, 3, {{{0, 0},{0, 1},{-1, 1},{0, -1},{-1, -1}},{{0, 0},{-1, 0},{-1, -1},{1, 0},{1, -1}},{{0, 0},{0, -1},{1, -1},{0, 1},{1, 1}},{{0, 0},{1, 0},{1, 1},{-1, 0},{-1, 1}},{{0, 0},{0, 1},{1, 1},{0, -1},{1, -1}},{{0, 0},{-1, 0},{-1, 1},{1, 0},{1, 1}},{{0, 0},{0, -1},{-1, -1},{0, 1},{-1, 1}},{{0, 0},{1, 0},{1, -1},{-1, 0},{-1, -1}}}, {1,2,3,4}),
	Block('o', 5, 2, 4, {{{0, -1},{0, 0},{1, 0},{0, 1},{0, 2}},{{1, 0},{0, 0},{0, 1},{-1, 0},{-2, 0}},{{0, 1},{0, 0},{-1, 0},{0, -1},{0, -2}},{{-1, 0},{0, 0},{0, -1},{1, 0},{2, 0}},{{0, -1},{0, 0},{-1, 0},{0, 1},{0, 2}},{{1, 0},{0, 0},{0, -1},{-1, 0},{-2, 0}},{{0, 1},{0, 0},{1, 0},{0, -1},{0, -2}},{{-1, 0},{0, 0},{0, 1},{1, 0},{2, 0}}}, {0,2,4}),
	Block('p', 5, 3, 3, {{{0, 0},{0, -1},{0, 1},{-1, 1},{1, 1}},{{0, 0},{1, 0},{-1, 0},{-1, -1},{-1, 1}},{{0, 0},{0, 1},{0, -1},{1, -1},{-1, -1}},{{0, 0},{-1, 0},{1, 0},{1, 1},{1, -1}},{{0, 0},{0, -1},{0, 1},{1, 1},{-1, 1}},{{0, 0},{1, 0},{-1, 0},{-1, 1},{-1, -1}},{{0, 0},{0, 1},{0, -1},{-1, -1},{1, -1}},{{0, 0},{-1, 0},{1, 0},{1, -1},{1, 1}}}, {1,3,4}),
	Block('q', 5, 3, 3, {{{0, 0},{1, 0},{2, 0},{0, -1},{0, -2}},{{0, 0},{0, 1},{0, 2},{1, 0},{2, 0}},{{0, 0},{-1, 0},{-2, 0},{0, 1},{0, 2}},{{0, 0},{0, -1},{0, -2},{-1, 0},{-2, 0}},{{0, 0},{-1, 0},{-2, 0},{0, -1},{0, -2}},{{0, 0},{0, -1},{0, -2},{1, 0},{2, 0}},{{0, 0},{1, 0},{2, 0},{0, 1},{0, 2}},{{0, 0},{0, 1},{0, 2},{-1, 0},{-2, 0}}}, {0,2,4}),
	Block('r', 5, 3, 3, {{{0, 0},{1, 0},{1, 1},{0, -1},{-1, -1}},{{0, 0},{0, 1},{-1, 1},{1, 0},{1, -1}},{{0, 0},{-1, 0},{-1, -1},{0, 1},{1, 1}},{{0, 0},{0, -1},{1, -1},{-1, 0},{-1, 1}},{{0, 0},{-1, 0},{-1, 1},{0, -1},{1, -1}},{{0, 0},{0, -1},{-1, -1},{1, 0},{1, 1}},{{0, 0},{1, 0},{1, -1},{0, 1},{-1, 1}},{{0, 0},{0, 1},{1, 1},{-1, 0},{-1, -1}}}, {0,1,2,3,4}),
	Block('s', 5, 3, 3, {{{0, 0},{1, 0},{1, 1},{-1, 0},{-1, -1}},{{0, 0},{0, 1},{-1, 1},{0, -1},{1, -1}},{{0, 0},{-1, 0},{-1, -1},{1, 0},{1, 1}},{{0, 0},{0, -1},{1, -1},{0, 1},{-1, 1}},{{0, 0},{-1, 0},{-1, 1},{1, 0},{1, -1}},{{0, 0},{0, -1},{-1, -1},{0, 1},{1, 1}},{{0, 0},{1, 0},{1, -1},{-1, 0},{-1, 1}},{{0, 0},{0, 1},{1, 1},{0, -1},{-1, -1}}}, {1,2,3,4}),
	Block('t', 5, 3, 3, {{{-1, -1},{-1, 0},{0, 0},{1, 0},{0, 1}},{{1, -1},{0, -1},{0, 0},{0, 1},{-1, 0}},{{1, 1},{1, 0},{0, 0},{-1, 0},{0, -1}},{{-1, 1},{0, 1},{0, 0},{0, -1},{1, 0}},{{1, -1},{1, 0},{0, 0},{-1, 0},{0, 1}},{{1, 1},{0, 1},{0, 0},{0, -1},{-1, 0}},{{-1, 1},{-1, 0},{0, 0},{1, 0},{0, -1}},{{-1, -1},{0, -1},{0, 0},{0, 1},{1, 0}}}, {0,1,3,4}),
	Block('u', 5, 3, 3, {{{0, 0},{1, 0},{0, 1},{-1, 0},{0, -1}},{{0, 0},{0, 1},{-1, 0},{0, -1},{1, 0}},{{0, 0},{-1, 0},{0, -1},{1, 0},{0, 1}},{{0, 0},{0, -1},{1, 0},{0, 1},{-1, 0}},{{0, 0},{-1, 0},{0, 1},{1, 0},{0, -1}},{{0, 0},{0, -1},{-1, 0},{0, 1},{1, 0}},{{0, 0},{1, 0},{0, -1},{-1, 0},{0, 1}},{{0, 0},{0, 1},{1, 0},{0, -1},{-1, 0}}}, {1,2,3,4})
};

void pbd_vec(const std::vector<int>& b)
{
    constexpr int W = 20;
    constexpr int H = 20;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int v = b[y * W + x];
            if (v == -1)
                std::cout << ". ";
            else
                std::cout << v << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

void pbd(const libpentobi_base::Board& b) {
   std::cout << b << std::endl;
}

std::unordered_map<std::string, int> pieceMap = {
  {"1", 0},
  {"2", 1},
  {"I3", 2},
  {"V3", 3},
  {"I4", 4},
  {"L4", 5},
  {"T4", 6},
  {"O", 7},
  {"Z4", 8},
  {"I5", 9},
  {"L5", 10},
  {"N", 11},
  {"P", 12},
  {"U", 13},
  {"Y", 14},
  {"T5", 15},
  {"V5", 16},
  {"W", 17},
  {"Z5", 18},
  {"F", 19},
  {"X", 20},
};
std::vector<TurnBaseMove> generateMap()
{
    const auto& bc=libpentobi_base::BoardConst::get(libpentobi_base::Variant::classic);
    int total_moves = bc.get_range();  
    std::vector<TurnBaseMove> result(total_moves);
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
              result[id] = TurnBaseMove(pieceId,rowCol->first,rowCol->second,r);
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
        if(ok&&tmp==outBoard){
          return {{r,c}};
        }
    }
    return std::nullopt;
}

std::vector<int> makeBoard(const Board& b){
    std::vector<int> v(20 * 20);
    const auto& geo = b.get_geometry();
    const auto& grid = b.get_point_state();
    for (auto p : geo)
    {
        unsigned col = geo.get_x(p);
        unsigned row = geo.get_y(p);

        PointState s = grid[p];
        int val = s.is_empty() ? -1 : s.to_int();   // -1 empty, 0–3 = colours

        v[row*20+col]= val;
    }
    return v;
}

std::unordered_map<std::string, int> reverseMap(std::vector<TurnBaseMove> moves){
  std::unordered_map<std::string, int> result;
  for(size_t i = 0 ; i < moves.size(); i++ ){
    TurnBaseMove move = moves[i];
    std::string key = std::to_string(move.pieceId) + ","  + std::to_string(move.row) + "," +  std::to_string(move.col) + "," + std::to_string(move.rotation);
    result[key] = i;
  } 
  return result;
}

int main()
{
    std::vector<TurnBaseMove> moves = generateMap();
    std::unordered_map<std::string, int> rm = reverseMap(moves);

    /* ---------- C++ FILE ---------- */

    std::ofstream out("cached_moves.cpp");
    if (!out) {
        std::cerr << "Failed to open cached_moves.cpp\n";
        return 1;
    }

    out << "// GENERATED FILE — DO NOT EDIT\n";
    out << "#include <vector>\n";
    out << "#include \"cached_moves.h\"\n";
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

    /* ---------- PYTHON FILE ---------- */

    std::ofstream py("cached_moves.py");
    if (!py) {
        std::cerr << "Failed to open cached_moves.py\n";
        return 1;
    }

    py << "# GENERATED FILE — DO NOT EDIT\n";
    py << "# Auto-generated from C++\n\n";
    py << "reverse_moves = {\n";

    for (const auto& [key, value] : rm) {
        py << "    \"" << key << "\": " << value << ",\n";
    }

    py << "}\n";
    py.close();

    std::cout << "Generated cached_moves.cpp with "
              << moves.size() << " entries\n";
    std::cout << "Generated cached_moves.py with "
              << rm.size() << " entries\n";

    return 0;
}

