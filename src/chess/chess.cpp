#include "chess.h"

Chess::Chess(){
    gameState = CASTLE_A1 | CASTLE_H1 | CASTLE_A8 | CASTLE_H8;
    enpassant = -1;
    halfMoves = 0;
    totalMoves = 0;
    colorTurn = WHITE;
    oppColor = BLACK;
    moveGenTime = 0;

    // Initialize piece lookup array
    for (int sq = 0; sq < 64; ++sq) {
        pieceAt[sq] = UNKNOWN;
    }
    
    for (int p = W_PAWN; p <= B_KING; ++p) {
        uint64_t bb = currentBoard[p];
        while (bb) {
            uint64_t sqBB = bb & -bb;
            int sq = __builtin_ctzll(sqBB);
            bb ^= sqBB;
            pieceAt[sq] = static_cast<Piece>(p);
        }
    }
}

// Returns the origin of the attackers to a square.
uint64_t Chess::attacksToSquare(Square sq, Piece color){
    Piece attColor = (color == WHITE) ? BLACK : WHITE;

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

    uint8_t from = pieceMove.getFrom();
    uint8_t to = pieceMove.getTo();
    uint8_t flags = pieceMove.getFlags();
    Piece pieceType = pieceAt[from];
    captureHistory[totalMoves] = pieceAt[to];

    // Update piece and color bitboards
    uint64_t fromBB = (i << from);
    uint64_t toBB = (i << to);
    uint64_t fromToBB =  fromBB ^ toBB;
    currentBoard[colorTurn] ^= fromToBB;
    currentBoard[pieceType] ^= fromToBB;
    pieceAt[from] = UNKNOWN;
    pieceAt[to] = pieceType;

    // Check if a rook move to disable castling
    if(pieceType == (colorTurn + W_ROOK)){
        switch(from){
            case A1:
                stateHistory[totalMoves] = gameState & CASTLE_A1;
                gameState &= ~CASTLE_A1;
                break;
            case H1:
                stateHistory[totalMoves] = gameState & CASTLE_H1;
                gameState &= ~CASTLE_H1;
                break;
            case A8:
                stateHistory[totalMoves] = gameState & CASTLE_A8;
                gameState &= ~CASTLE_A8;
                break;
            case H8:
                stateHistory[totalMoves] = gameState & CASTLE_H8;
                gameState &= ~CASTLE_H8;
                break;
            default: break;
        }
    }
    else if(pieceType == (colorTurn + W_KING)){
        uint8_t newState = colorTurn == WHITE ? (CASTLE_A1 | CASTLE_H1) : (CASTLE_A8 | CASTLE_H8);
        stateHistory[totalMoves] = gameState & newState;
        gameState &= ~newState;
    }

    switch(flags) {
        case KING_CASTLE: {
            Square rookFrom = A1;
            Square rookTo = A1;

            if(colorTurn == WHITE) {
                rookFrom = H1;
                rookTo = F1;
            }
            else {
                rookFrom = H8;
                rookTo = F8;
            }

            fromBB = (i << rookFrom);
            toBB = (i << rookTo);
            fromToBB = fromBB ^ toBB;
            currentBoard[colorTurn] ^= fromToBB;
            currentBoard[colorTurn + W_ROOK] ^= fromToBB;
            pieceAt[rookFrom] = UNKNOWN;
            pieceAt[rookTo] = (Piece)(colorTurn + W_ROOK);
            break;
        }
        case QUEEN_CASTLE: {
            Square rookFrom = A1;
            Square rookTo = A1;

            if(colorTurn == WHITE) {
                rookFrom = A1;
                rookTo = D1;
            }
            else {
                rookFrom = A8;
                rookTo = D8;
            }

            fromBB = (i << rookFrom);
            toBB = (i << rookTo);
            fromToBB = fromBB ^ toBB;
            currentBoard[colorTurn] ^= fromToBB;
            currentBoard[colorTurn + W_ROOK] ^= fromToBB;
            pieceAt[rookFrom] = UNKNOWN;
            pieceAt[rookTo] = (Piece)(colorTurn + W_ROOK);
            break;
        }
        case CAPTURE_MOVE: {
            currentBoard[oppColor] ^= toBB;
            currentBoard[captureHistory[totalMoves]] ^= toBB;

            if(captureHistory[totalMoves] == (oppColor + W_ROOK)) {
                switch(to){
                    case A1:
                        stateHistory[totalMoves] = gameState & CASTLE_A1;
                        gameState &= ~CASTLE_A1;
                        break;
                    case H1:
                        stateHistory[totalMoves] = gameState & CASTLE_H1;
                        gameState &= ~CASTLE_H1;
                        break;
                    case A8:
                        stateHistory[totalMoves] = gameState & CASTLE_A8;
                        gameState &= ~CASTLE_A8;
                        break;
                    case H8:
                        stateHistory[totalMoves] = gameState & CASTLE_H8;
                        gameState &= ~CASTLE_H8;
                        break;
                    default: break;
                }
            }
            break;
        }
        default: break;
    }

    moveHistory[totalMoves] = pieceMove;
    totalMoves++;

    Piece temp = colorTurn;
    colorTurn = oppColor;
    oppColor = temp;

    occupiedBoard = currentBoard[WHITE] | currentBoard[BLACK];
}

// Reverse the last move made. Used to check if the king is capture after a move, to known that is illegal.
void Chess::undoMove(){
    uint64_t i = 1;

    Move pieceMove = moveHistory[totalMoves - 1];

    uint8_t from = pieceMove.getFrom();
    uint8_t to = pieceMove.getTo();
    uint8_t flags = pieceMove.getFlags();
    Piece pieceType = pieceAt[to];

    uint64_t fromBB = (i << from);
    uint64_t toBB = (i << to);
    uint64_t fromToBB = fromBB ^ toBB;
    currentBoard[oppColor] ^= fromToBB;
    currentBoard[pieceType] ^= fromToBB;
    pieceAt[from] = pieceType;
    pieceAt[to] = UNKNOWN;

    switch(flags) {
        case KING_CASTLE: {
            Square rookFrom = A1;
            Square rookTo = A1;

            if(oppColor == WHITE) {
                rookFrom = H1;
                rookTo = F1;
            }
            else {
                rookFrom = H8;
                rookTo = F8;
            }

            fromBB = (i << rookFrom);
            toBB = (i << rookTo);
            fromToBB = fromBB ^ toBB;
            currentBoard[oppColor] ^= fromToBB;
            currentBoard[oppColor + W_ROOK] ^= fromToBB;
            pieceAt[rookFrom] = (Piece)(oppColor + W_ROOK);
            pieceAt[rookTo] = UNKNOWN;
            break;
        }
        case QUEEN_CASTLE: {
            Square rookFrom = A1;
            Square rookTo = A1;

            if(oppColor == WHITE) {
                rookFrom = A1;
                rookTo = D1;
            }
            else {
                rookFrom = A8;
                rookTo = D8;
            }

            fromBB = (i << rookFrom);
            toBB = (i << rookTo);
            fromToBB = fromBB ^ toBB;
            currentBoard[oppColor] ^= fromToBB;
            currentBoard[oppColor + W_ROOK] ^= fromToBB;
            pieceAt[rookFrom] = (Piece)(oppColor + W_ROOK);
            pieceAt[rookTo] = UNKNOWN;
            break;
        }
        case CAPTURE_MOVE: {
            currentBoard[colorTurn] ^= toBB;
            currentBoard[captureHistory[totalMoves - 1]] ^= toBB;
            pieceAt[to] = captureHistory[totalMoves - 1];
            break;
        }
        default: break;
    }

    // We recover the state
    if(stateHistory[totalMoves - 1] > 0){
        gameState |= stateHistory[totalMoves - 1];
        gameState &= ~GAME_OVER;
    }

    totalMoves--;

    Piece temp = colorTurn;
    colorTurn = oppColor;
    oppColor = temp;

    occupiedBoard = currentBoard[WHITE] | currentBoard[BLACK];
}

// Generates a list of moves for a given piece moveboard.
inline void Chess::getMovesFromBB(MoveList &moveList, uint64_t bitboard, Square squareFrom, uint8_t flag){
    while(bitboard > 0){
        Square squareTo = (Square)__builtin_ctzll(bitboard);
        bitboard &= bitboard - 1;

        Move newMove(squareFrom, squareTo, flag);
        moveList.add(newMove);
    }
}

// Generates all the moves without checking if the king can be capture.
MoveList Chess::getPseudoLegalMoves(){
    uint64_t playerBoard = currentBoard[colorTurn];
    MoveList moveList;
    Square kingSquare = A1;

    int sq;
    while(playerBoard){
        int sq = __builtin_ctzll(playerBoard);
        playerBoard &= playerBoard - 1;

        uint64_t moves = 0;
        uint64_t captures = 0;
        Piece pieceType = pieceAt[sq];

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
            getMovesFromBB(moveList, moves, (Square)sq, QUIET_MOVE);
        }

        if (captures > 0) {
            getMovesFromBB(moveList, captures, (Square)sq, CAPTURE_MOVE);
        }
    }

    // Check for castling rights, only if the king is not in check.
    if(colorTurn == WHITE) {
        if(gameState & CASTLE_A1){
            Move newMove(E1, C1, QUEEN_CASTLE);
            moveList.add(newMove);
        }
        if(gameState & CASTLE_H1){
            Move newMove(E1, G1, KING_CASTLE);
            moveList.add(newMove);
        }
    }
    else {
        if(gameState & CASTLE_A8){
            Move newMove(E8, C8, QUEEN_CASTLE);
            moveList.add(newMove);
        }
        if(gameState & CASTLE_H8){
            Move newMove(E8, G8, KING_CASTLE);
            moveList.add(newMove);
        }
    }

    return moveList;
}

bool Chess::isLegal(Move move, Square kingSquare) {
    uint8_t flags = move.getFlags();
    uint8_t from = move.getFrom();
    uint8_t to = move.getTo();

    if((flags == KING_CASTLE) || (flags == QUEEN_CASTLE)) {
        if(attacksToSquare(kingSquare, colorTurn)) return false;
        
        bool pathClear = false;
        switch(to){
            case C1: {
                bool path = (occupiedBoard & 14) == 0;
                pathClear = path && (attacksToSquare(D1, colorTurn) == 0) && (attacksToSquare(C1, colorTurn) == 0);
                break;
            }
            case G1: {
                bool path = (occupiedBoard & 96) == 0;
                pathClear = path && (attacksToSquare(F1, colorTurn) == 0) && (attacksToSquare(G1, colorTurn) == 0);
                break;
            }
            case C8: {
                bool path = (occupiedBoard & 1008806316530991104U) == 0;
                pathClear = path && (attacksToSquare(D8, colorTurn) == 0) && (attacksToSquare(C8, colorTurn) == 0);
                break;
            }
            case G8: {
                bool path = (occupiedBoard & 6917529027641081856U) == 0;
                pathClear = path && (attacksToSquare(F8, colorTurn) == 0) && (attacksToSquare(G8, colorTurn) == 0);
                break;
            }
            default: break;
        }

        return pathClear;
    }
    else if(pieceAt[from] == (Piece)(colorTurn + W_KING)) {
        makeMove(move);
        bool kingSafe = attacksToSquare((Square)move.getTo(), oppColor) == 0;
        undoMove();

        return kingSafe;
    }

    makeMove(move);
    // OppColor because after making the move colors switch
    bool kingSafe = attacksToSquare(kingSquare, oppColor) == 0;
    undoMove();

    return kingSafe;
}

MoveList Chess::getLegalMoves(){
    auto t1 = std::chrono::high_resolution_clock::now();
    
    MoveList pseudoList = getPseudoLegalMoves();
    MoveList legalMoves;
    
    Square kingSquare = (Square)__builtin_ctzll(currentBoard[colorTurn + W_KING]);
    // Makes the move and checks if the king is attack to get the legal moves.
    for(Move m: pseudoList){
        if(isLegal(m, kingSquare)) {
            legalMoves.add(m);
        }
    }

    if(legalMoves.count == 0){
        gameState |= GAME_OVER;
        stateHistory[totalMoves - 1] |= GAME_OVER;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    moveGenTime += ms_int.count();

    return legalMoves;
}

PerftResults Chess::perft(int depth) {
    PerftResults results;

    MoveList moves = getLegalMoves();

    if (gameState & GAME_OVER) {
        if(moveHistory[totalMoves  - 1].getFlags() == CAPTURE_MOVE) {
            results.captures++;
        }

        // std::cout << "\nCheckmate: " + getFen() + "\n"<< std::endl;

        results.totalCount++;
        results.checkmates++;
        return results;
    }

    if(depth == 0) {
        if(moveHistory[totalMoves - 1].getFlags() == CAPTURE_MOVE) {
            results.captures++;
        }

        results.totalCount++;
        return results;
    }

    for(Move m : moves) {
        makeMove(m);
        results.add(perft(depth - 1));
        undoMove();
    }

    return results;
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