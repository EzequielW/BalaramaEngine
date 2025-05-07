#ifndef __CHESS_LOGIC__
#define __CHESS_LOGIC__

#include <cstdint>
#include <vector>
#include <chrono>
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include "generator.h"
#include "move_structs.h"

typedef struct PerftResults {
    long long totalCount = 0;
    long captures = 0;
    long checks = 0;
    long checkmates = 0;
    long enpassant = 0;

    void add(PerftResults other) {
        totalCount += other.totalCount;
        captures += other.captures;
        checks += other.checks;
        checkmates += other.checkmates;
        enpassant += other.enpassant;
    }
} PerftResults;

class Chess{
public:
    Generator generator;
    Move moveHistory[512] = {};
    uint8_t stateHistory[512] = { 0 };
    Piece captureHistory[512] = { UNKNOWN };
    Square enpassant[512] = { A1 };

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
    uint8_t gameState;
    // Moves since capture or pawn move
    int halfMoves;
    int totalMoves;

    Piece colorTurn;
    Piece oppColor;

    long long moveGenTime = 0;

    Chess();
    void makeMove(Move pieceMove);
    void undoMove();
    uint64_t attacksToSquare(Square sq, Piece color);
    inline void getMovesFromBB(MoveList &moveList, uint64_t bitboard, Square squareFrom, uint8_t flag);
    MoveList getPseudoLegalMoves();
    bool isLegal(Move move, Square kingSquare);
    MoveList getLegalMoves();
    PerftResults perft(int depth);
    std::vector<Piece> getCurrentBoard(); // To do remove, pieceAt already covers this
    Piece getSquareColor(int sq);
    std::string getFen();
    Piece getPieceAt(Square from);
    #ifdef __EMSCRIPTEN__
    emscripten::val getLegalMovesAsJsArray();
    #endif
};

#endif // __CHESS_LOGIC__
