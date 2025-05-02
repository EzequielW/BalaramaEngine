#include "chess.h"

Chess::Chess(){
    gameState = CASTLE_A1 | CASTLE_H1 | CASTLE_A8 | CASTLE_H8;
    enpassant = -1;
    halfMoves = 0;
    totalMoves = 1;
    colorTurn = WHITE;
    oppColor = BLACK;

    // Initialize piece lookup array
    for (int sq = 0; sq < 64; ++sq) {
        pieceAt[sq] = UNKNOWN;
    }
    
    for (int p = W_PAWN; p <= B_KING; ++p) {
        uint64_t bb = currentBoard[p];
        while (bb) {
            uint64_t sqBB = bb & -bb;
            int sq = generator.bitScanForward(sqBB);
            bb ^= sqBB;
            pieceAt[sq] = static_cast<Piece>(p);
        }
    }
}

// Returns the origin of the attackers to a square.
uint64_t Chess::attacksToSquare(Square sq, Piece color){
    Piece attColor;
    if(color == WHITE){
        attColor = BLACK;
    }
    else{
        attColor = WHITE;
    }

    uint64_t bishopBlockers = generator.bishopMoves[sq] & occupiedBoard;
    uint64_t rookBlockers = generator.rookMoves[sq] & occupiedBoard;
    uint64_t diagonals = currentBoard[attColor + W_BISHOP] | currentBoard[attColor + W_QUEEN];
    uint64_t lines = currentBoard[attColor + W_ROOK] | currentBoard[attColor + W_QUEEN];

    uint64_t pawnAttack = generator.pawnAttacks[color][sq] & currentBoard[attColor + W_PAWN];
    uint64_t kightAttack = generator.knightMoves[sq] & currentBoard[attColor + W_KNIGHT];
    uint64_t bishopAttack = generator.bishopMoveboard[sq][bishopBlockers] & diagonals;
    uint64_t rookAttack = generator.rookMoveboard[sq][rookBlockers] & lines;
    uint64_t kingAttack = generator.kingMoves[sq] & currentBoard[attColor + W_KING];

    uint64_t attacks = pawnAttack | kightAttack | bishopAttack | rookAttack | kingAttack;

    return attacks;
}

void Chess::makeMove(Move pieceMove){
    uint64_t i = 1;

    // Update piece and color bitboards
    uint64_t fromBB = (i << pieceMove.squareFrom);
    uint64_t toBB = (i << pieceMove.squareTo);
    uint64_t fromToBB =  fromBB ^ toBB;
    currentBoard[pieceMove.pieceColor] ^= fromToBB;
    currentBoard[pieceMove.pieceType] ^= fromToBB;
    pieceAt[pieceMove.squareFrom] = UNKNOWN;
    pieceAt[pieceMove.squareTo] = pieceMove.pieceType;

    // Check if a rook move to disable castling
    if(pieceMove.pieceType == (pieceMove.pieceColor + W_ROOK)){
        switch(pieceMove.squareFrom){
            case A1:
                pieceMove.moveState = gameState & CASTLE_A1;
                gameState &= ~CASTLE_A1;
                break;
            case H1:
                pieceMove.moveState = gameState & CASTLE_H1;
                gameState &= ~CASTLE_H1;
                break;
            case A8:
                pieceMove.moveState = gameState & CASTLE_A8;
                gameState &= ~CASTLE_A8;
                break;
            case H8:
                pieceMove.moveState = gameState & CASTLE_H8;
                gameState &= ~CASTLE_H8;
                break;
            default: break;
        }
    }
    else if(pieceMove.pieceType == (pieceMove.pieceColor + W_KING)){
        pieceMove.moveState = gameState & (pieceMove.pieceColor == WHITE ? (CASTLE_A1 | CASTLE_H1) : (CASTLE_A8 | CASTLE_H8));
        gameState &= pieceMove.pieceColor == WHITE ? ~(CASTLE_A1 | CASTLE_H1) : ~(CASTLE_A8 | CASTLE_H8);
    }

    // Check if its castle and move the rook
    if(pieceMove.castleFrom){
        toBB = (i << pieceMove.castleFrom);
        fromToBB = (i << pieceMove.castleTo) ^ toBB;
        currentBoard[pieceMove.cPieceColor] ^= fromToBB;
        currentBoard[pieceMove.cPieceType] ^= fromToBB;
        pieceAt[pieceMove.castleFrom] = UNKNOWN;
        pieceAt[pieceMove.castleTo] = pieceMove.cPieceType;
    }
    // Check for capture and update the bitboards
    else if(pieceMove.cPieceColor != UNKNOWN){
        currentBoard[pieceMove.cPieceColor] ^= toBB;
        currentBoard[pieceMove.cPieceType] ^= toBB;
    }

    moveHistory.push_back(pieceMove);

    Piece temp = colorTurn;
    colorTurn = oppColor;
    oppColor = temp;

    occupiedBoard = currentBoard[WHITE] | currentBoard[BLACK];
}

// Reverse the last move made. Used to check if the king is capture after a move, to known that is illegal.
void Chess::undoMove(){
    uint64_t i = 1;

    Move pieceMove = moveHistory.back();

    uint64_t fromBB = (i << pieceMove.squareFrom);
    uint64_t toBB = (i << pieceMove.squareTo);
    uint64_t fromToBB = fromBB ^ toBB;
    currentBoard[pieceMove.pieceColor] ^= fromToBB;
    currentBoard[pieceMove.pieceType] ^= fromToBB;
    pieceAt[pieceMove.squareFrom] = pieceMove.pieceType;
    pieceAt[pieceMove.squareTo] = UNKNOWN;

    if(pieceMove.castleFrom){
        toBB = (i << pieceMove.castleFrom);
        fromToBB = (i << pieceMove.castleTo) ^ toBB;
        currentBoard[pieceMove.cPieceColor] ^= fromToBB;
        currentBoard[pieceMove.cPieceType] ^= fromToBB;
        pieceAt[pieceMove.castleFrom] = pieceMove.cPieceType;
        pieceAt[pieceMove.castleTo] = UNKNOWN;
    }
    else if(pieceMove.cPieceColor != UNKNOWN){
        currentBoard[pieceMove.cPieceColor] ^= toBB;
        currentBoard[pieceMove.cPieceType] ^= toBB;
        pieceAt[pieceMove.squareTo] = pieceMove.cPieceType;
    }

    // We recover the state
    if(pieceMove.moveState > 0){
        gameState |= pieceMove.moveState;
        gameState &= ~GAME_OVER;
    }

    moveHistory.pop_back();

    Piece temp = colorTurn;
    colorTurn = oppColor;
    oppColor = temp;

    occupiedBoard = currentBoard[WHITE] | currentBoard[BLACK];
}

// Generates a list of moves for a given piece moveboard.
inline void Chess::getMovesFromBB(MoveList &moveList, uint64_t bitboard, Square squareFrom, Piece pieceColor, Piece pieceType, bool capture){
    Piece cPieceColor = UNKNOWN;
    Piece cPieceType = UNKNOWN;
    if(capture){
        cPieceColor = oppColor;
    }

    while(bitboard > 0){
        int squareTo = __builtin_ctzll(bitboard);
        bitboard &= bitboard - 1;

        if(capture){
            cPieceType = pieceAt[squareTo];
        }

        Move pieceMove = {squareFrom, (Square)squareTo, pieceColor, pieceType, cPieceColor, cPieceType, UNKNOWN, (Square)0, (Square)0};
        moveList.add(pieceMove);
    }
}

// Generates all the moves without checking if the king can be capture.
MoveList Chess::getPseudoLegalMoves(){
    uint64_t playerBoard = currentBoard[colorTurn];
    MoveList moveList;
    
    Square kingSquare = A1;

    bool correctColor[15] = {
        colorTurn == WHITE, // WHITE
        colorTurn == BLACK, // BLACK
        colorTurn == WHITE, // W_PAWN
        colorTurn == BLACK, // B_PAWN
        colorTurn == WHITE, // W_KNIGHT
        colorTurn == BLACK, // B_KNIGHT
        colorTurn == WHITE, // W_BISHOP
        colorTurn == BLACK, // B_BISHOP
        colorTurn == WHITE, // W_ROOK
        colorTurn == BLACK, // B_ROOK
        colorTurn == WHITE, // W_QUEEN
        colorTurn == BLACK, // B_QUEEN
        colorTurn == WHITE, // W_KING
        colorTurn == BLACK, // B_KING
        false  // UNKNOWN
    };

    constexpr uint64_t FILE_A = 0x0101010101010101ULL;
    constexpr uint64_t FILE_H = 0x8080808080808080ULL;

    int sq;
    while(playerBoard){
        int sq = __builtin_ctzll(playerBoard);
        playerBoard &= playerBoard - 1;

        uint64_t moves = 0;
        uint64_t captures = 0;
        Piece pieceType = pieceAt[sq];

        if(!correctColor[pieceType]) continue;

        switch(pieceType) {
            case W_PAWN: case B_PAWN: {
                uint64_t blockers = generator.rookMoves[sq] & occupiedBoard;
                moves = generator.pawnMoves[colorTurn][sq] & generator.rookMoveboard[sq][blockers];
                captures = generator.pawnAttacks[colorTurn][sq];
                break;
            }
            case W_KNIGHT: case B_KNIGHT: {
                moves = generator.knightMoves[sq];
                captures = moves;
                break;
            }
            case W_BISHOP: case B_BISHOP: {
                uint64_t blockers = generator.bishopMoves[sq] & occupiedBoard;
                moves = generator.bishopMoveboard[sq][blockers];
                captures = moves;
                break;
            }
            case W_ROOK: case B_ROOK: {
                uint64_t blockers = generator.rookMoves[sq] & occupiedBoard;
                moves = generator.rookMoveboard[sq][blockers];
                captures = moves;
                break;
            }
            case W_QUEEN: case B_QUEEN: {
                uint64_t bishopBlockers = generator.bishopMoves[sq] & occupiedBoard;
                uint64_t rookBlockers = generator.rookMoves[sq] & occupiedBoard;
                moves = generator.bishopMoveboard[sq][bishopBlockers];
                moves |= generator.rookMoveboard[sq][rookBlockers];
                captures = moves;
                break;
            }
            case W_KING: case B_KING: {
                moves = generator.kingMoves[sq];
                captures = moves;
                kingSquare = (Square)sq;
                break;
            }
            default: break;
        }

        moves &= ~occupiedBoard;
        captures &= currentBoard[oppColor];

        if (moves > 0) {
            getMovesFromBB(moveList, moves, (Square)sq, colorTurn, pieceType, false);
        }

        if (captures > 0) {
            getMovesFromBB(moveList, captures, (Square)sq, colorTurn, pieceType, true);
        }
    }

    uint64_t attToKing = attacksToSquare(kingSquare, colorTurn);
    // Check for castling rights, only if the king is not in check.
    if(attToKing == 0){
        // Also check if the castling path is clear.
        if(colorTurn == WHITE){
            bool path = (currentBoard[WHITE] & 14) == 0;
            path = path && (attacksToSquare(D1, colorTurn) == 0) && (attacksToSquare(C1, colorTurn) == 0);
            if((gameState & CASTLE_A1) && path){
                Move castle = {E1, C1, WHITE, W_KING, WHITE, W_ROOK, UNKNOWN, A1, D1};
                moveList.add(castle);
            }
            path = (currentBoard[WHITE] & 96) == 0;
            path = path && (attacksToSquare(F1, colorTurn) == 0) && (attacksToSquare(G1, colorTurn) == 0);
            if((gameState & CASTLE_H1) && path){
                Move castle = {E1, G1, WHITE, W_KING, WHITE, W_ROOK, UNKNOWN, H1, F1};
                moveList.add(castle);
            }
        }
        else{
            bool path = (currentBoard[BLACK] & 1008806316530991104U) == 0;
            path = path && (attacksToSquare(D8, colorTurn) == 0) && (attacksToSquare(C8, colorTurn) == 0);
            if((gameState & CASTLE_A8) && path){
                Move castle = {E8, C8, BLACK, B_KING, BLACK, B_ROOK, UNKNOWN, A8, D8};
                moveList.add(castle);
            }
            path = (currentBoard[BLACK] & 6917529027641081856U) == 0;
            path = path && (attacksToSquare(F8, colorTurn) == 0) && (attacksToSquare(G8, colorTurn) == 0);
            if((gameState & CASTLE_H8) && path){
                Move castle = {E8, G8, BLACK, B_KING, BLACK, B_ROOK, UNKNOWN, H8, F8};
                moveList.add(castle);
            }
        }
    }

    return moveList;
}

MoveList Chess::getLegalMoves(){
    MoveList pseudoList = getPseudoLegalMoves();
    MoveList legalMoves;
    Piece currentColor = colorTurn;

    // Makes the move and checks if the king is attack to get the legal moves.
    for(Move m: pseudoList){
        makeMove(m);
        Square kingSquare = (Square)generator.bitScanForward(currentBoard[currentColor + W_KING]);
        if(attacksToSquare(kingSquare, currentColor) == 0) {
            legalMoves.add(m);
        }
        undoMove();
    }

    if(legalMoves.count == 0){
        gameState |= GAME_OVER;
        moveHistory.back().moveState |= GAME_OVER;
    }

    return legalMoves;
}

// Returns a vector of size 64 representing the board. Is no used by the engine so it can be created in real time.
std::vector<Piece> Chess::getCurrentBoard() {
    std::vector<Piece> board;
    uint64_t currentSq = 1;

    for (int i = 0; i < 64; i++) {
        if ((currentSq << i) & currentBoard[W_PAWN]) {
            board.push_back(W_PAWN);
        }
        else if ((currentSq << i) & currentBoard[B_PAWN]) {
            board.push_back(B_PAWN);
        }
        else if ((currentSq << i) & currentBoard[W_KNIGHT]) {
            board.push_back(W_KNIGHT);
        }
        else if ((currentSq << i) & currentBoard[B_KNIGHT]) {
            board.push_back(B_KNIGHT);
        }
        else if ((currentSq << i) & currentBoard[W_BISHOP]) {
            board.push_back(W_BISHOP);
        }
        else if ((currentSq << i) & currentBoard[B_BISHOP]) {
            board.push_back(B_BISHOP);
        }
        else if ((currentSq << i) & currentBoard[W_ROOK]) {
            board.push_back(W_ROOK);
        }
        else if ((currentSq << i) & currentBoard[B_ROOK]) {
            board.push_back(B_ROOK);
        }
        else if ((currentSq << i) & currentBoard[W_QUEEN]) {
            board.push_back(W_QUEEN);
        }
        else if ((currentSq << i) & currentBoard[B_QUEEN]) {
            board.push_back(B_QUEEN);
        }
        else if ((currentSq << i) & currentBoard[W_KING]) {
            board.push_back(W_KING);
        }
        else if ((currentSq << i) & currentBoard[B_KING]) {
            board.push_back(B_KING);
        }
        else {
            board.push_back(UNKNOWN);
        }
    }

    return board;
}

Piece Chess::getSquareColor(int sq) {
    uint64_t square = 1;
    Piece sqColor;

    if ((square << sq) & currentBoard[WHITE]) {
        sqColor = WHITE;
    }
    else if((square << sq) & currentBoard[BLACK]){
        sqColor = BLACK;
    }
    else {
        sqColor = UNKNOWN;
    }

    return sqColor;
}

std::string Chess::getFen() {
    std::vector<Piece> board = getCurrentBoard();
    std::string fen = "";

    // Set pieces position
    for(int i = 7; i >= 0; i--) {
        int emptyCount = 0;

        for(int j = 0; j < 8; j++) {
            int index = i * 8 + j;

            char squareValue = pieceToString(board[index]);

            if(squareValue == '-') {
                emptyCount++;
            }
            else if(emptyCount > 0) {
                char emptySpaces = '0' + emptyCount;
                fen += emptySpaces;
                emptyCount = 0;
            }
            
            if(squareValue != '-') {
                fen += squareValue;
            }
        }

        if(emptyCount > 0) {
            char emptySpaces = '0' + emptyCount;
            fen += emptySpaces;
            emptyCount = 0;
        }

        if(i != 0) {
            fen += '/';
        }
    }

    // Color turn to move
    fen += ' ';
    fen += pieceToString(colorTurn);

    // Castling rights
    fen += ' ';
    std::string castlingRights = "";
    castlingRights += (gameState & CASTLE_H1) ? "K" : "";
    castlingRights += (gameState & CASTLE_A1) ? "Q" : "";
    castlingRights += (gameState & CASTLE_H8) ? "k" : "";
    castlingRights += (gameState & CASTLE_A8) ? "q" : "";

    if(castlingRights.empty()) {
        fen += '-';
    }
    else {
        fen += castlingRights;
    }

    // En passant
    fen += ' ';
    if(enpassant > 0) {
        fen += squareToString((Square) enpassant);
    }
    else {
        fen += '-';
    }

    // Half moves
    fen += ' ' + std::to_string(halfMoves);

    // Total moves
    fen += ' ' + std::to_string(totalMoves);

    return fen;
}

#ifdef __EMSCRIPTEN__
emscripten::val Chess::getBoardAsJsArray() {
    std::vector<Piece> board = getCurrentBoard();
    emscripten::val jsArray = emscripten::val::array();

    // Convert std::vector<Piece> to a JavaScript array
    for (size_t i = 0; i < board.size(); ++i) {
        jsArray.call<void>("push", emscripten::val(board[i]));  // Push each piece
    }

    return jsArray;
}

emscripten::val Chess::getLegalMovesAsJsArray() {
    std::vector<Move> legalMoves = getLegalMoves();
    emscripten::val jsArray = emscripten::val::array();

    // Convert std::vector<Move> to a JavaScript array
    for (size_t i = 0; i < legalMoves.size(); ++i) {
        jsArray.call<void>("push", emscripten::val(legalMoves[i]));  // Push each move
    }

    return jsArray;
}
#endif