#ifndef __CHESS_LOGIC__
#define __CHESS_LOGIC__

#include <cstdint>
#include <vector>
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include "generator.h"
#include "move_structs.h"

const int GAME_OVER = 1;
const int CASTLE_A1 = (1 << 1);
const int CASTLE_H1 = (1 << 2);
const int CASTLE_A8 = (1 << 3);
const int CASTLE_H8 = (1 << 4);

class Chess{
public:
    Generator generator;
    std::array<Move, 512> moveHistory = {};

    // Array representing the current state of the board
    uint64_t currentBoard[14] = {
        65535,                  // White pieces
        18446462598732840960U,   // Black pieces
        65280,                  // White pawns
        71776119061217280U,      // Black pawns
        66,                     // White knights
        4755801206503243776U,    // Black knights
        36,                     // White bishops
        2594073385365405696U,    // Black bishops
        129,                    // White rooks
        9295429630892703744U,    // Black rooks
        8,                      // White queens
        576460752303423488U,     // Black queens
        16,                     // White king
        1152921504606846976U     // Black king
    };
    uint64_t occupiedBoard = 18446462598732906495U;
    // For faster lookup while move generating
    Piece pieceAt[64]; 

    // Tell us about castling rights and who is going to move next
    int gameState;
    int enpassant;
    // Moves since capture or pawn move
    int halfMoves;
    int totalMoves;

    Piece colorTurn;
    Piece oppColor;

    Chess();
    void makeMove(Move pieceMove);
    void undoMove();
    uint64_t attacksToSquare(Square sq, Piece color);
    inline void getMovesFromBB(MoveList &moveList, uint64_t bitboard, Square squareFrom, Piece pieceColor, Piece pieceType, bool capture);
    MoveList getPseudoLegalMoves();
    bool isLegal(Move move, Square kingSquare);
    MoveList getLegalMoves();
    std::vector<Piece> getCurrentBoard(); // To do remove, pieceAt already covers this
    Piece getSquareColor(int sq);
    std::string getFen();
    #ifdef __EMSCRIPTEN__
    emscripten::val getBoardAsJsArray();
    emscripten::val getLegalMovesAsJsArray();
    #endif
};

#endif // __CHESS_LOGIC__
