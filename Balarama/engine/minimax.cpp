#include "minimax.h"

Minimax::Minimax() {
    std::vector<int> wpawnScore{
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-20,-20, 10, 10,  5,
        5, -5,-10,  0,  0,-10, -5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        0, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };
    std::vector<int> wknightScore{
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };
    std::vector<int> wbishopScore{
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };
    std::vector<int> wrookScore{
        0,  0,  0,  5,  5,  0,  0,  0,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        5, 10, 10, 10, 10, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };
    std::vector<int> wqueenScore{
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -10,  5,  5,  5,  5,  5,  0,-10,
        0,  0,  5,  5,  5,  5,  0, -5,
        -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    std::vector<int> wkingScore{
        20, 30, 10,  0,  0, 10, 30, 20,
        20, 20,  0,  0,  0,  0, 20, 20,
        10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };
    std::vector<int> bpawnScore(64, 0);
    std::vector<int> bknightScore(64, 0);
    std::vector<int> bbishopScore(64, 0);
    std::vector<int> brookScore(64, 0);
    std::vector<int> bqueenScore(64, 0);
    std::vector<int> bkingScore(64, 0);

	// Reversing scores for the black side
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			int index = i * 8 + j;
			int reverseIndex = (7 - i) * 8 + j;

			bpawnScore[index] = wpawnScore[reverseIndex];
			bknightScore[index] = wknightScore[reverseIndex];
			bbishopScore[index] = wbishopScore[reverseIndex];
			brookScore[index] = wrookScore[reverseIndex];
			bqueenScore[index] = wqueenScore[reverseIndex];
			bkingScore[index] = wkingScore[reverseIndex];
		}
	}

    pieceScores[W_PAWN] = wpawnScore;
    pieceScores[W_KNIGHT] = wknightScore;
    pieceScores[W_BISHOP] = wbishopScore;
    pieceScores[W_ROOK] = wrookScore;
    pieceScores[W_QUEEN] = wqueenScore;
    pieceScores[W_KING] = wkingScore;
    pieceScores[B_PAWN] = bpawnScore;
    pieceScores[B_KNIGHT] = bknightScore;
    pieceScores[B_BISHOP] = bbishopScore;
    pieceScores[B_ROOK] = brookScore;
    pieceScores[B_QUEEN] = bqueenScore;
    pieceScores[B_KING] = bkingScore;
}

float Minimax::heuristicEval(Chess* chess) {
	int nodeScore[14] = { 0 };
    float nodeEvaluation = 0.0f;

    if (chess->gameState & GAME_OVER) {
        if (chess->colorTurn == WHITE) {
            return INFINITE_EVAL;
        }
        else {
            return -INFINITE_EVAL;
        }
    }

	// Get number of moves each has
	if (chess->colorTurn == WHITE) {
        nodeScore[WHITE] = chess->getLegalMoves().size();
        chess->colorTurn = BLACK;
        chess->oppColor = WHITE;
        nodeScore[BLACK] = chess->getLegalMoves().size();
        chess->colorTurn = WHITE;
        chess->oppColor = BLACK;
	}
	else {
        nodeScore[BLACK] = chess->getLegalMoves().size();
        chess->colorTurn = WHITE;
        chess->oppColor = BLACK;
        nodeScore[WHITE] = chess->getLegalMoves().size();
        chess->colorTurn = BLACK;
        chess->oppColor = WHITE;
	}

    nodeEvaluation += 10 * (nodeScore[WHITE] - nodeScore[BLACK]);

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
        + 350 * (wknightSize - bknightSize) + (wpawnSize - bpawnSize);


	uint64_t boardCopy[14];
	std::copy(chess->currentBoard, chess->currentBoard + 14, boardCopy);

    // Sum all the pieces scores depending on the square they are placed
	bool piecesChange = true;
	uint64_t temp = 1;
	while (piecesChange) {
		piecesChange = false;
        for (int i = (int)W_PAWN; i < (int)UNKNOWN; i++) {
            if (boardCopy[(Piece)i] > 0) {
                int square = chess->generator.bitScanForward(boardCopy[(Piece)i]);
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

    return nodeEvaluation;
}

Evaluation Minimax::searchABPruning(Chess* chess, int depth, int& steps, float alpha, float beta) {
    steps += 1;

    std::vector<Move> moveList = chess->getLegalMoves();
    if (depth == 0 || (chess->gameState & GAME_OVER)) {
        Evaluation eval;
        eval.result = heuristicEval(chess);
        return eval;
    }
    
    if (chess->colorTurn == WHITE) {
        Evaluation maxEval;
        maxEval.result = -INFINITE_EVAL;
        float currentEval = -INFINITE_EVAL;

        for (Move m : moveList) {
            chess->makeMove(m);
            currentEval = searchABPruning(chess, depth - 1, steps, alpha, beta).result;

            if (currentEval > maxEval.result) {
                maxEval.result = currentEval;
                maxEval.move = m;
                alpha = std::max(alpha, maxEval.result);
                if (alpha >= beta) {
                    chess->undoMove();
                    break;
                }
            }

            chess->undoMove();
        }

        return maxEval;
    }
    else {
        Evaluation minEval;
        minEval.result = INFINITE_EVAL;
        float currentEval = INFINITE_EVAL;

        for (Move m : moveList) {
            chess->makeMove(m);
            currentEval = searchABPruning(chess, depth - 1, steps, alpha, beta).result;

            if (currentEval < minEval.result) {
                minEval.result = currentEval;
                minEval.move = m;
                beta = std::min(beta, minEval.result);
                if (alpha >= beta) {
                    chess->undoMove();
                    break;
                }
            }

            chess->undoMove();
        }

        return minEval;
    }
}