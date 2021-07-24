#ifndef __MOVE_STRUCTS__
#define __MOVE_STRUCTS__

enum Piece{
    WHITE,      BLACK,
    W_PAWN,     B_PAWN,
    W_KNIGHT,   B_KNIGHT,
    W_BISHOP,   B_BISHOP,
    W_ROOK,     B_ROOK,
    W_QUEEN,    B_QUEEN,
    W_KING,     B_KING,
    UNKNOWN
};

enum Square{
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

typedef struct Move{
    Square squareFrom;
    Square squareTo;
    Piece pieceColor;
    Piece pieceType;
    Piece cPieceColor;
    Piece cPieceType;
    Piece promotion;
    Square castleFrom;
    Square castleTo;
    int moveState = 0;
}Move;
#endif // __MOVE_STRUCTS__
