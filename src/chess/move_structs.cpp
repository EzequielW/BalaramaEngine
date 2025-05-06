#include "move_structs.h"

char pieceToString(Piece piece) {
    switch(piece) {
        case Piece::WHITE: return 'w';
        case Piece::BLACK: return 'b';
        case Piece::W_PAWN: return 'P';
        case Piece::B_PAWN: return 'p';
        case Piece::W_KNIGHT: return 'N';
        case Piece::B_KNIGHT: return 'n';
        case Piece::W_BISHOP: return 'B';
        case Piece::B_BISHOP: return 'b';
        case Piece::W_ROOK: return 'R';
        case Piece::B_ROOK: return 'r';
        case Piece::W_QUEEN: return 'Q';
        case Piece::B_QUEEN: return 'q';
        case Piece::W_KING: return 'K';
        case Piece::B_KING: return 'k';
        case Piece::UNKNOWN: return '-';
        default: return '-';
    }
}

std::string squareToString(Square square) {
    switch(square) {
        case Square::A1: return "a1";
        case Square::A2: return "a2";
        case Square::A3: return "a3";
        case Square::A4: return "a4";
        case Square::A5: return "a5";
        case Square::A6: return "a6";
        case Square::A7: return "a7";
        case Square::A8: return "a8";
        case Square::B1: return "b1";
        case Square::B2: return "b2";
        case Square::B3: return "b3";
        case Square::B4: return "b4";
        case Square::B5: return "b5";
        case Square::B6: return "b6";
        case Square::B7: return "b7";
        case Square::B8: return "b8";
        case Square::C1: return "c1";
        case Square::C2: return "c2";
        case Square::C3: return "c3";
        case Square::C4: return "c4";
        case Square::C5: return "c5";
        case Square::C6: return "c6";
        case Square::C7: return "c7";
        case Square::C8: return "c8";
        case Square::D1: return "d1";
        case Square::D2: return "d2";
        case Square::D3: return "d3";
        case Square::D4: return "d4";
        case Square::D5: return "d5";
        case Square::D6: return "d6";
        case Square::D7: return "d7";
        case Square::D8: return "d8";
        case Square::E1: return "e1";
        case Square::E2: return "e2";
        case Square::E3: return "e3";
        case Square::E4: return "e4";
        case Square::E5: return "e5";
        case Square::E6: return "e6";
        case Square::E7: return "e7";
        case Square::E8: return "e8";
        case Square::F1: return "f1";
        case Square::F2: return "f2";
        case Square::F3: return "f3";
        case Square::F4: return "f4";
        case Square::F5: return "f5";
        case Square::F6: return "f6";
        case Square::F7: return "f7";
        case Square::F8: return "f8";
        case Square::G1: return "g1";
        case Square::G2: return "g2";
        case Square::G3: return "g3";
        case Square::G4: return "g4";
        case Square::G5: return "g5";
        case Square::G6: return "g6";
        case Square::G7: return "g7";
        case Square::G8: return "g8";
        case Square::H1: return "h1";
        case Square::H2: return "h2";
        case Square::H3: return "h3";
        case Square::H4: return "h4";
        case Square::H5: return "h5";
        case Square::H6: return "h6";
        case Square::H7: return "h7";
        case Square::H8: return "h8";
        default: return "unknown";
    }
}

JSMove getJSMove(Move move) {
    JSMove jsMove;
    jsMove.from = (Square)move.getFrom();
    jsMove.to = (Square)move.getTo();
    jsMove.flags = (MoveFlag)move.getFlags();

    return jsMove;
}