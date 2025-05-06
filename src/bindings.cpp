#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <memory>
#include "chess/chess.h"
#include "engine/minimax.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(piece_enum) {
    function("pieceToString", &pieceToString);
    function("squareToString", &squareToString);

    enum_<Piece>("Piece")
        .value("WHITE", Piece::WHITE)
        .value("BLACK", Piece::BLACK)
        .value("W_PAWN", Piece::W_PAWN)
        .value("B_PAWN", Piece::B_PAWN)
        .value("W_KNIGHT", Piece::W_KNIGHT)
        .value("B_KNIGHT", Piece::B_KNIGHT)
        .value("W_BISHOP", Piece::W_BISHOP)
        .value("B_BISHOP", Piece::B_BISHOP)
        .value("W_ROOK", Piece::W_ROOK)
        .value("B_ROOK", Piece::B_ROOK)
        .value("W_QUEEN", Piece::W_QUEEN)
        .value("B_QUEEN", Piece::B_QUEEN)
        .value("W_KING", Piece::W_KING)
        .value("B_KING", Piece::B_KING)
        .value("UNKNOWN", Piece::UNKNOWN);
}

EMSCRIPTEN_BINDINGS(square_enum) {
    enum_<Square>("Square")
        .value("a1", Square::A1)
        .value("a2", Square::A2)
        .value("a3", Square::A3)
        .value("a4", Square::A4)
        .value("a5", Square::A5)
        .value("a6", Square::A6)
        .value("a7", Square::A7)
        .value("a8", Square::A8)
        .value("b1", Square::B1)
        .value("b2", Square::B2)
        .value("b3", Square::B3)
        .value("b4", Square::B4)
        .value("b5", Square::B5)
        .value("b6", Square::B6)
        .value("b7", Square::B7)
        .value("b8", Square::B8)
        .value("c1", Square::C1)
        .value("c2", Square::C2)
        .value("c3", Square::C3)
        .value("c4", Square::C4)
        .value("c5", Square::C5)
        .value("c6", Square::C6)
        .value("c7", Square::C7)
        .value("c8", Square::C8)
        .value("d1", Square::D1)
        .value("d2", Square::D2)
        .value("d3", Square::D3)
        .value("d4", Square::D4)
        .value("d5", Square::D5)
        .value("d6", Square::D6)
        .value("d7", Square::D7)
        .value("d8", Square::D8)
        .value("e1", Square::E1)
        .value("e2", Square::E2)
        .value("e3", Square::E3)
        .value("e4", Square::E4)
        .value("e5", Square::E5)
        .value("e6", Square::E6)
        .value("e7", Square::E7)
        .value("e8", Square::E8)
        .value("f1", Square::F1)
        .value("f2", Square::F2)
        .value("f3", Square::F3)
        .value("f4", Square::F4)
        .value("f5", Square::F5)
        .value("f6", Square::F6)
        .value("f7", Square::F7)
        .value("f8", Square::F8)
        .value("g1", Square::G1)
        .value("g2", Square::G2)
        .value("g3", Square::G3)
        .value("g4", Square::G4)
        .value("g5", Square::G5)
        .value("g6", Square::G6)
        .value("g7", Square::G7)
        .value("g8", Square::G8)
        .value("h1", Square::H1)
        .value("h2", Square::H2)
        .value("h3", Square::H3)
        .value("h4", Square::H4)
        .value("h5", Square::H5)
        .value("h6", Square::H6)
        .value("h7", Square::H7)
        .value("h8", Square::H8);
}

EMSCRIPTEN_BINDINGS(move_class) {
    enum_<MoveFlag>("MoveFlag")
        .value("QUIET_MOVE", MoveFlag::QUIET_MOVE)
        .value("DOUBLE_PAWN", MoveFlag::DOUBLE_PAWN)
        .value("KING_CASTLE", MoveFlag::KING_CASTLE)
        .value("QUEEN_CASTLE", MoveFlag::QUEEN_CASTLE)
        .value("CAPTURE_MOVE", MoveFlag::CAPTURE_MOVE)
        .value("EP_CAPTURE", MoveFlag::EP_CAPTURE)
        .value("KNIGHT_PROMOTION", MoveFlag::KNIGHT_PROMOTION)
        .value("BISHOP_PROMOTION", MoveFlag::BISHOP_PROMOTION)
        .value("ROOK_PROMOTION", MoveFlag::ROOK_PROMOTION)
        .value("QUEEN_PROMOTION", MoveFlag::QUEEN_PROMOTION)
        .value("KNIGHT_PROMOTION_C", MoveFlag::KNIGHT_PROMOTION_C)
        .value("BISHOP_PROMOTION_C", MoveFlag::BISHOP_PROMOTION_C)
        .value("ROOK_PROMOTION_C", MoveFlag::ROOK_PROMOTION_C)
        .value("QUEEN_PROMOTION_C", MoveFlag::QUEEN_PROMOTION_C);

    class_<Move>("Move")
        .constructor<>()
        .constructor<Square, Square, uint8_t>()
        .function("getTo", &Move::getTo)
        .function("getFrom", &Move::getFrom)
        .function("getFlags", &Move::getFlags);

    value_object<JSMove>("JSMove")
        .field("from", &JSMove::from)
        .field("to", &JSMove::to)
        .field("flags", &JSMove::flags);

    function("getJSMove", &getJSMove);
}

EMSCRIPTEN_BINDINGS(chess_engine_module) {
    class_<Chess>("Chess")
        .constructor<>()
        .function("makeMove", &Chess::makeMove)
        .function("getFen", &Chess::getFen)
        .function("getPieceAt", &Chess::getPieceAt)
        .function("getLegalMovesAsJsArray", &Chess::getLegalMovesAsJsArray);

    class_<Minimax>("Minimax")
        .constructor<>()
        .function("searchABPruning", &Minimax::searchABPruning);
    
    value_object<FinalEvaluation>("FinalEvaluation")
        .field("result", &FinalEvaluation::result)
        .field("move", &FinalEvaluation::move)
        .field("steps", &FinalEvaluation::steps)
        .field("heuristicTime", &FinalEvaluation::heuristicTime)
        .field("moveGenTime", &FinalEvaluation::moveGenTime);
}

#endif