#ifndef __MOVE_STRUCTS__
#define __MOVE_STRUCTS__

#include <cstdint>
#include <string>
#include <array>

enum Piece: uint8_t {
    WHITE,      BLACK,
    W_PAWN,     B_PAWN,
    W_KNIGHT,   B_KNIGHT,
    W_BISHOP,   B_BISHOP,
    W_ROOK,     B_ROOK,
    W_QUEEN,    B_QUEEN,
    W_KING,     B_KING,
    UNKNOWN
};

enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

enum Direction{
    NORTH,
    EAST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH,
    WEST,
    SOUTH_EAST,
    SOUTH_WEST
};

enum MoveFlag : uint8_t {
    QUIET_MOVE = 0,
    DOUBLE_PAWN = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE_MOVE = 4,
    EP_CAPTURE = 5,
    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION = 9,
    ROOK_PROMOTION = 10,
    QUEEN_PROMOTION = 11,
    KNIGHT_PROMOTION_C = 12,
    BISHOP_PROMOTION_C = 13,
    ROOK_PROMOTION_C = 14,
    QUEEN_PROMOTION_C = 15
};

const uint8_t GAME_OVER = 1;
const uint8_t CASTLE_A1 = (1 << 1);
const uint8_t CASTLE_H1 = (1 << 2);
const uint8_t CASTLE_A8 = (1 << 3);
const uint8_t CASTLE_H8 = (1 << 4);

// Moves are represented by 16 usigned integers, 12 bits for square from/to, and 4 bits for flags
typedef struct Move {
    uint16_t move = 0;

    Move() {
        move = 0;
    }

    Move(Square from, Square to, uint8_t flags) {
        move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
    }

    uint8_t getTo() const {return move & 0x3f;}
    uint8_t getFrom() const {return (move >> 6) & 0x3f;}
    uint8_t getFlags() const {return (move >> 12) & 0x0f;}
} Move;

constexpr size_t MAX_MOVES = 256;

typedef struct MoveList {
    std::array<Move, MAX_MOVES> moves;
    size_t count = 0;

    void add(const Move& move) {
        if (count < MAX_MOVES)
            moves[count++] = move;
    }

    void pop() {
        if (count > 0)
            count--;
    }

    void clear() { count = 0; }

    Move* begin() { return moves.data(); }
    Move* end() { return moves.data() + count; }

    const Move* begin() const { return moves.data(); }
    const Move* end() const { return moves.data() + count; }
} MoveList;

char pieceToString(Piece piece);
std::string squareToString(Square square);

#endif // __MOVE_STRUCTS__
