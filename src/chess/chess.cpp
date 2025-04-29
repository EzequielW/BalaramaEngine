#include "chess.h"

Chess::Chess(){
    gameState = CASTLE_A1 | CASTLE_H1 | CASTLE_A8 | CASTLE_H8;
    enpassant = -1;
    halfMoves = 0;
    totalMoves = 1;
    colorTurn = WHITE;
    oppColor = BLACK;
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

    // We recover the state
    if(pieceMove.moveState > 0){
        gameState |= pieceMove.moveState;
        gameState &= ~GAME_OVER;
    }

    if(pieceMove.castleFrom){
        toBB = (i << pieceMove.castleFrom);
        fromToBB = (i << pieceMove.castleTo) ^ toBB;
        currentBoard[pieceMove.cPieceColor] ^= fromToBB;
        currentBoard[pieceMove.cPieceType] ^= fromToBB;
    }
    else if(pieceMove.cPieceColor != UNKNOWN){
        currentBoard[pieceMove.cPieceColor] ^= toBB;
        currentBoard[pieceMove.cPieceType] ^= toBB;
    }

    moveHistory.pop_back();

    Piece temp = colorTurn;
    colorTurn = oppColor;
    oppColor = temp;

    occupiedBoard = currentBoard[WHITE] | currentBoard[BLACK];
}

// Generates a list of moves for a given piece moveboard.
void Chess::getMovesFromBB(std::vector<Move> &moveList, uint64_t bitboard, Square squareFrom, Piece pieceColor, Piece pieceType, bool capture){
    Piece cPieceColor = UNKNOWN;
    Piece cPieceType = UNKNOWN;
    if(capture){
        cPieceColor = oppColor;
    }

    uint64_t i = 1;
    while(bitboard > 0){
        int squareTo = generator.bitScanForward(bitboard);
        if(capture){
            if(currentBoard[oppColor + W_PAWN] & (i << squareTo)){
                cPieceType = (Piece)(cPieceColor + W_PAWN);
            }
            else if(currentBoard[oppColor + W_KNIGHT] & (i << squareTo)){
                cPieceType = (Piece)(cPieceColor + W_KNIGHT);
            }
            else if(currentBoard[oppColor + W_BISHOP] & (i << squareTo)){
                cPieceType = (Piece)(cPieceColor + W_BISHOP);
            }
            else if(currentBoard[oppColor + W_ROOK] & (i << squareTo)){
                cPieceType = (Piece)(cPieceColor + W_ROOK);
            }
            else if(currentBoard[oppColor + W_QUEEN] & (i << squareTo)){
                cPieceType = (Piece)(cPieceColor + W_QUEEN);
            }
            else if(currentBoard[oppColor + W_KING] & (i << squareTo)){
                cPieceType = (Piece)(oppColor + W_KING);
            }
        }

        Move pieceMove = {squareFrom, (Square)squareTo, pieceColor, pieceType, cPieceColor, cPieceType, UNKNOWN, (Square)0, (Square)0};
        moveList.push_back(pieceMove);
        bitboard ^= (i << squareTo);
    }
}

// Generates all the moves without checking if the king can be capture.
std::vector<Move> Chess::getPseudoLegalMoves(){
    uint64_t playerBoard = currentBoard[colorTurn];
    std::vector<Move> moveList;
    std::vector<Move> nonCaptureMoves;
    Square kingSquare = A1;

    while(playerBoard > 0){
        int sq = generator.bitScanForward(playerBoard);
        uint64_t i = 1;
        uint64_t moves = 0;
        uint64_t captures = 0;
        Piece pieceType = UNKNOWN;

        if(currentBoard[colorTurn + W_PAWN] & (i << sq)){
            moves = generator.pawnMoves[colorTurn][sq];
            captures = generator.pawnAttacks[colorTurn][sq];
            pieceType = (Piece)(colorTurn + W_PAWN);
        }
        else if(currentBoard[colorTurn + W_KNIGHT] & (i << sq)){
            moves = generator.knightMoves[sq];
            captures = moves;
            pieceType = (Piece)(colorTurn + W_KNIGHT);
        }
        else if(currentBoard[colorTurn + W_BISHOP] & (i << sq)){
            uint64_t blockers = generator.bishopMoves[sq] & occupiedBoard;
            moves = generator.bishopMoveboard[sq][blockers];
            captures = moves;
            pieceType = (Piece)(colorTurn + W_BISHOP);
        }
        else if(currentBoard[colorTurn + W_ROOK] & (i << sq)){
            uint64_t blockers = generator.rookMoves[sq] & occupiedBoard;
            moves = generator.rookMoveboard[sq][blockers];
            captures = moves;
            pieceType = (Piece)(colorTurn + W_ROOK);
        }
        else if(currentBoard[colorTurn + W_QUEEN] & (i << sq)){
            uint64_t bishopBlockers = generator.bishopMoves[sq] & occupiedBoard;
            uint64_t rookBlockers = generator.rookMoves[sq] & occupiedBoard;
            moves = generator.bishopMoveboard[sq][bishopBlockers];
            moves |= generator.rookMoveboard[sq][rookBlockers];
            captures = moves;
            pieceType = (Piece)(colorTurn + W_QUEEN);
        }
        else if(currentBoard[colorTurn + W_KING] & (i << sq)){
            moves = generator.kingMoves[sq];
            captures = moves;
            pieceType = (Piece)(colorTurn + W_KING);
            kingSquare = (Square)sq;
        }

        moves &= ~occupiedBoard;
        captures &= currentBoard[oppColor];

        if (moves > 0) {
            getMovesFromBB(nonCaptureMoves, moves, (Square)sq, colorTurn, pieceType, false);
        }

        if (captures > 0) {
            getMovesFromBB(moveList, captures, (Square)sq, colorTurn, pieceType, true);
        }

        playerBoard ^= (i << sq);
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
                moveList.push_back(castle);
            }
            path = (currentBoard[WHITE] & 96) == 0;
            path = path && (attacksToSquare(F1, colorTurn) == 0) && (attacksToSquare(G1, colorTurn) == 0);
            if((gameState & CASTLE_H1) && path){
                Move castle = {E1, G1, WHITE, W_KING, WHITE, W_ROOK, UNKNOWN, H1, F1};
                moveList.push_back(castle);
            }
        }
        else{
            bool path = (currentBoard[BLACK] & 1008806316530991104U) == 0;
            path = path && (attacksToSquare(D8, colorTurn) == 0) && (attacksToSquare(C8, colorTurn) == 0);
            if((gameState & CASTLE_A8) && path){
                Move castle = {E8, C8, BLACK, B_KING, BLACK, B_ROOK, UNKNOWN, A8, D8};
                moveList.push_back(castle);
            }
            path = (currentBoard[BLACK] & 6917529027641081856U) == 0;
            path = path && (attacksToSquare(F8, colorTurn) == 0) && (attacksToSquare(G8, colorTurn) == 0);
            if((gameState & CASTLE_H8) && path){
                Move castle = {E8, G8, BLACK, B_KING, BLACK, B_ROOK, UNKNOWN, H8, F8};
                moveList.push_back(castle);
            }
        }
    }

    // Add captures first to improve minmax efficiency 
    moveList.insert(moveList.end(), nonCaptureMoves.begin(), nonCaptureMoves.end());

    return moveList;
}

std::vector<Move> Chess::getLegalMoves(){
    std::vector<Move> pseudoList = getPseudoLegalMoves();
    std::vector<Move> legalMoves;
    Piece currentColor = colorTurn;

    // Makes the move and checks if the king is attack to get the legal moves.
    for(Move m: pseudoList){
        makeMove(m);
        Square kingSquare = (Square)generator.bitScanForward(currentBoard[currentColor + W_KING]);
        if(attacksToSquare(kingSquare, currentColor) == 0) {
            legalMoves.push_back(m);
        }
        undoMove();
    }

    if(legalMoves.empty()){
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