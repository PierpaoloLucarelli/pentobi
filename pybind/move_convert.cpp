#include "move_convert.h"
#include "libpentobi_base/BoardConst.h"
#include "libpentobi_base/Move.h"
#include "libpentobi_base/Piece.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_base/Variant.h"

using namespace libpentobi_base;

std::vector<TurnBaseMove> generateMap(){ 
  Game game(Variant::classic);
  game.init();
  const Board& bd = game.get_board();
  const libpentobi_base::BoardConst& bc = 
    libpentobi_base::BoardConst::get(libpentobi_base::Variant::classic);

  for (Move::IntType id = 0; id < bc.get_range(); ++id)
  {
      Move mv(id);
      std::cout << bd.to_string(mv) << std::endl;
      Piece p = bd.get_move_piece(mv);
      auto pInfo = bd.get_piece_info(p);
      int pieceId = pieceMap.at(pInfo.get_name());

  } 
};


TurnBaseMove findMove(std::vector<int> board){
  
}
