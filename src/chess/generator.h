#ifndef __GENERATOR__
#define __GENERATOR__
#include <cstdint>
#include <map>
#include <vector>
#include <iostream>

#include "move_structs.h"

// The generator class is use to initialize all the useful data for fast move generation
class Generator{
public:
    // Moves that dont consider blockers
    uint64_t pawnMoves[2][64];
    uint64_t knightMoves[64];
    uint64_t kingMoves[64];
    uint64_t pawnAttacks[2][64];

    // Contains directional rays from all squares without blockers
    uint64_t rayMoves[64][8];

    // Contains rook and bishop moves create with the rays. The moves at the edge of the board are not used.
    uint64_t rookMoves[64];
    uint64_t bishopMoves[64];

    // This maps have as index the bitboard resulting from & operation between the moveboard and the blockerboard.
    // Then returns the bitboard with the allowed moves. Captures and friendly pieces are filtered afterwards.
    std::map<int, std::map<uint64_t, uint64_t>> rookMoveboard;
    std::map<int, std::map<uint64_t, uint64_t>> bishopMoveboard;

    // This maps work in a similar way to moveboards, but instead consider the moves after the first blocker.
    // They are used to check for absolute pins.
    std::map<int, std::map<uint64_t, uint64_t>> rookXrays;
    std::map<int, std::map<uint64_t, uint64_t>> bishopXrays;

    // Constructor calls all the generation methods
    Generator();

    // Search the index of least and most significant bit respectively
    int bitScanForward(uint64_t n);
    int bitScanReverse(uint64_t n);
    // Returns the number of bit set for some number
    int bitCountSet(uint64_t n);

    // Utility functions to check relevant files(columns in chess)
    bool isAFile(Square square){ return square % 8 == 0; }
    bool isBFile(Square square){ return (square - 1) % 8 == 0; }
    bool isGFile(Square square){ return (square + 2) % 8 == 0; }
    bool isHFile(Square square){ return (square + 1) % 8 == 0; }

    // Generation of the most basic moves for each pieces. Ray moves must initialize before rook and bishop.
    void genPawnMoves();
    void genKnightMoves();
    void genKingMoves();
    void genPawnAttacks();
    void genRayMoves();
    void genRookMoves();
    void genBishopMoves();

    // Generates a unique blockerboard with an index and either a rook or bishop moveboard(without blockers).
    // The number of indexes is determined by the number of 1 bits in the moveboard.
    uint64_t genBlockerBoard(int index, uint64_t rayMask);

    void genRookMoveboards();
    void genBishopMoveboards();
    void genRookXrays();
    void genBishopXrays();
};
#endif // __GENERATOR__
