#include "minimax.h"
#include <chrono>
#include <algorithm>

Minimax::Minimax() {
    int wpawnScore[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-20,-20, 10, 10,  5,
        5, -5,-10,  0,  0,-10, -5,  5,
        0,  0,  0, 50, 50,  0,  0,  0,
        5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        0, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };
    int wknightScore[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };
    int wbishopScore[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };
    int wrookScore[64] = {
        0,  0,  0,  5,  5,  0,  0,  0,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        5, 10, 10, 10, 10, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };
    int wqueenScore[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -10,  5,  5,  5,  5,  5,  0,-10,
        0,  0,  5,  5,  5,  5,  0, -5,
        -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    int wkingScore[64] = {
        20, 30, 10,  0,  0, 10, 30, 20,
        20, 20,  0,  0,  0,  0, 20, 20,
        10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };
    int bpawnScore[64] = {0};
    int bknightScore[64] = {0};
    int bbishopScore[64] = {0};
    int brookScore[64] = {0};
    int bqueenScore[64] = {0};
    int bkingScore[64] = {0};

	// Reversing scores for the black side
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			int index = i * 8 + j;
			int reverseIndex = (7 - i) * 8 + (7 - j);

			bpawnScore[index] = wpawnScore[reverseIndex];
			bknightScore[index] = wknightScore[reverseIndex];
			bbishopScore[index] = wbishopScore[reverseIndex];
			brookScore[index] = wrookScore[reverseIndex];
			bqueenScore[index] = wqueenScore[reverseIndex];
			bkingScore[index] = wkingScore[reverseIndex];
		}
	}

    std::copy(std::begin(wpawnScore), std::end(wpawnScore), std::begin(pieceScores[W_PAWN]));
    std::copy(std::begin(wknightScore), std::end(wknightScore), std::begin(pieceScores[W_KNIGHT]));
    std::copy(std::begin(wbishopScore), std::end(wbishopScore), std::begin(pieceScores[W_BISHOP]));
    std::copy(std::begin(wrookScore), std::end(wrookScore), std::begin(pieceScores[W_ROOK]));
    std::copy(std::begin(wqueenScore), std::end(wqueenScore), std::begin(pieceScores[W_QUEEN]));
    std::copy(std::begin(wkingScore), std::end(wkingScore), std::begin(pieceScores[W_KING]));
    std::copy(std::begin(bpawnScore), std::end(bpawnScore), std::begin(pieceScores[B_PAWN]));
    std::copy(std::begin(bknightScore), std::end(bknightScore), std::begin(pieceScores[B_KNIGHT]));
    std::copy(std::begin(bbishopScore), std::end(bbishopScore), std::begin(pieceScores[B_BISHOP]));
    std::copy(std::begin(brookScore), std::end(brookScore), std::begin(pieceScores[B_ROOK]));
    std::copy(std::begin(bqueenScore), std::end(bqueenScore), std::begin(pieceScores[B_QUEEN]));
    std::copy(std::begin(bkingScore), std::end(bkingScore), std::begin(pieceScores[B_KING]));
}

float Minimax::heuristicEval(std::shared_ptr<Chess> chess) {
    // auto t1 = std::chrono::high_resolution_clock::now();

	int nodeScore[14] = { 0 };
    float nodeEvaluation = 0.0f;

    if (chess->gameState & GAME_OVER) {
        // auto t2 = std::chrono::high_resolution_clock::now();
        // auto ms_int = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        // heuristicTime += ms_int.count();
        if (chess->colorTurn == WHITE) {
            return -INFINITE_EVAL;
        }
        else {
            return INFINITE_EVAL;
        }
    }

	//// Get number of moves each has
	//if (chess->colorTurn == WHITE) {
 //       nodeScore[WHITE] = totalMoves;
 //       chess->colorTurn = BLACK;
 //       chess->oppColor = WHITE;
 //       nodeScore[BLACK] = chess->getPseudoLegalMoves().size();
 //       chess->colorTurn = WHITE;
 //       chess->oppColor = BLACK;
	//}
	//else {
 //       nodeScore[BLACK] = totalMoves;
 //       chess->colorTurn = WHITE;
 //       chess->oppColor = BLACK;
 //       nodeScore[WHITE] = chess->getPseudoLegalMoves().size();
 //       chess->colorTurn = BLACK;
 //       chess->oppColor = WHITE;
	//}

 //   nodeEvaluation += 10 * (nodeScore[WHITE] - nodeScore[BLACK]);

	// Evaluation depending on the number of pieces each side has
	int wpawnSize = chess->generator.bitCountSet(chess->currentBoard[W_PAWN]);
	int wknightSize = chess->generator.bitCountSet(chess->currentBoard[W_KNIGHT]);
	int wbishopSize = chess->generator.bitCountSet(chess->currentBoard[W_BISHOP]);
	int wrookSize = chess->generator.bitCountSet(chess->currentBoard[W_ROOK]);
	int wqueenSize = chess->generator.bitCountSet(chess->currentBoard[W_QUEEN]);
	int bpawnSize = chess->generator.bitCountSet(chess->currentBoard[B_PAWN]);
	int bknightSize = chess->generator.bitCountSet(chess->currentBoard[B_KNIGHT]);
	int bbishopSize = chess->generator.bitCountSet(chess->currentBoard[B_BISHOP]);
	int brookSize = chess->generator.bitCountSet(chess->currentBoard[B_ROOK]);
	int bqueenSize = chess->generator.bitCountSet(chess->currentBoard[B_QUEEN]);

    nodeEvaluation += 1000 * (wqueenSize - bqueenSize) + 525 * (wrookSize - brookSize) + 350 * (wbishopSize - bbishopSize)
        + 350 * (wknightSize - bknightSize) + 100 * (wpawnSize - bpawnSize);


	uint64_t boardCopy[14];
	std::copy(chess->currentBoard, chess->currentBoard + 14, boardCopy);

    // Sum all the pieces scores depending on the square they are placed
	bool piecesChange = true;
	uint64_t temp = 1;
	while (piecesChange) {
		piecesChange = false;
        for (int i = (int)W_PAWN; i < (int)UNKNOWN; i++) {
            if (boardCopy[(Piece)i] > 0) {
                int square = __builtin_ctzll(boardCopy[(Piece)i]);
                nodeScore[(Piece)i] += pieceScores[(Piece)i][square];
                boardCopy[(Piece)i] ^= (temp << square);
                piecesChange = true;
            }
        }
	}

    nodeEvaluation += nodeScore[W_PAWN] - nodeScore[B_PAWN];
    nodeEvaluation += nodeScore[W_KNIGHT] - nodeScore[B_KNIGHT];
    nodeEvaluation += nodeScore[W_BISHOP] - nodeScore[B_BISHOP];
    nodeEvaluation += nodeScore[W_ROOK] - nodeScore[B_ROOK];
    nodeEvaluation += nodeScore[W_QUEEN] - nodeScore[B_QUEEN];
    nodeEvaluation += nodeScore[W_KING] - nodeScore[B_KING];
    nodeEvaluation /= 100;

    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto ms_int = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    // heuristicTime += ms_int.count();

    return nodeEvaluation;
}

float Minimax::quiescenceSearch(std::shared_ptr<Chess> chess, float alpha, float beta, int depth) {
    steps += 1;
    MoveList moves = chess->getLegalMoves();

    float bestValue = heuristicEval(chess);

    if(depth == 0 || (chess->gameState & GAME_OVER)) {
        return bestValue;
    }

    if(chess->colorTurn == WHITE) {
        if(bestValue >= beta) {
            return bestValue;
        }
        alpha = std::max(alpha, bestValue);
    } 
    else {
        if(bestValue <= alpha) {
            return bestValue;
        }
        beta = std::min(beta, bestValue);
    }
    

    for(Move move : moves) {
        Square kingSq = (Square)(chess->colorTurn + W_KING);
        bool isCheck = chess->attacksToSquare(kingSq, chess->colorTurn);
        if(move.getFlags() == CAPTURE_MOVE || isCheck) {
            chess->makeMove(move);
            float value = quiescenceSearch(chess, alpha, beta, depth - 1);
            chess->undoMove();
            
            if(chess->colorTurn == WHITE) {
                if(value >= beta) {
                    return value;
                }
                alpha = std::max(alpha, value);
            } 
            else {
                if(value <= alpha) {
                    return value;
                }
                beta = std::min(beta, value);
            }
        }
    }

    return bestValue;
}

FinalEvaluation Minimax::searchABPruning(Chess chess, int depth) {
    steps = 0;
    heuristicTime = 0;
    chess.moveGenTime = 0;
    float alpha = -INFINITE_EVAL;
    float beta = INFINITE_EVAL;

    std::shared_ptr<Chess> chessRef = std::make_shared<Chess>(chess);

    Evaluation evaluation = searchABPruningExec(chessRef, depth, alpha, beta);

    FinalEvaluation finalEvaluation;
    finalEvaluation.result = evaluation.result;
    finalEvaluation.move = evaluation.move;
    finalEvaluation.steps = steps;
    finalEvaluation.heuristicTime = heuristicTime;
    finalEvaluation.moveGenTime = chessRef->moveGenTime;
    std::copy(std::begin(evaluation.moveTree), std::end(evaluation.moveTree), std::begin(finalEvaluation.moveTree));
    return finalEvaluation;
}

Evaluation Minimax::searchABPruningExec(std::shared_ptr<Chess> chess, int depth, float alpha, float beta) {
    steps += 1;

    if (depth == 0) {
        // chess->getLegalMoves();
        Evaluation eval;
        // eval.result = heuristicEval(chess);
        eval.result = quiescenceSearch(chess, alpha, beta, 5);
        return eval;
    }

    MoveList moveList = chess->getLegalMoves();
    std::sort(moveList.begin(), moveList.end(), [](const Move& a, const Move& b) {
        bool killerMove = a.getFlags() == CAPTURE_MOVE && b.getFlags() != CAPTURE_MOVE;

        return killerMove;
    });

    if(chess->gameState & GAME_OVER) {
        Evaluation eval;
        eval.result = heuristicEval(chess);
        return eval;
    }
    
    if (chess->colorTurn == WHITE) {
        Evaluation maxEval;
        maxEval.result = -INFINITE_EVAL;
        Evaluation currentEval;
        currentEval.result = -INFINITE_EVAL;

        for (Move m : moveList) {
            chess->makeMove(m);
            currentEval = searchABPruningExec(chess, depth - 1, alpha, beta);

            if (currentEval.result >= maxEval.result) {
                maxEval.result = currentEval.result;
                maxEval.move = m;
                maxEval.moveTree[depth] = currentEval.moveTree[depth - 1];
                
                // Non leaf node
                if(currentEval.move.move != 0) {
                    std::copy(std::begin(currentEval.moveTree), std::end(currentEval.moveTree), std::begin(maxEval.moveTree));
                }
                maxEval.moveTree[depth] = m;

                if (maxEval.result >= beta) {
                    chess->undoMove();
                    break;
                }
                alpha = std::max(alpha, maxEval.result);
            }

            chess->undoMove();
        }

        return maxEval;
    }
    else {
        Evaluation minEval;
        minEval.result = INFINITE_EVAL;
        Evaluation currentEval;
        currentEval.result = INFINITE_EVAL;

        for (Move m : moveList) {
            chess->makeMove(m);
            currentEval = searchABPruningExec(chess, depth - 1, alpha, beta);

            if (currentEval.result <= minEval.result) {
                minEval.result = currentEval.result;
                minEval.move = m;
                minEval.moveTree[depth] = currentEval.moveTree[depth - 1];
                
                // Non leaf node
                if(currentEval.move.move != 0) {
                    std::copy(std::begin(currentEval.moveTree), std::end(currentEval.moveTree), std::begin(minEval.moveTree));
                }
                minEval.moveTree[depth] = m;
                
                if (minEval.result <= alpha) {
                    chess->undoMove();
                    break;
                }
                beta = std::min(beta, minEval.result);
            }

            chess->undoMove();
        }

        return minEval;
    }
}