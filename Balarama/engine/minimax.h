#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include <vector>
#include <iterator>
#include <cstdint>
#include <map>

#include "../chess/chess.h"

const float INFINITE_EVAL = 1000.0f;

typedef struct {
    float result;
    Move move;
}Evaluation;

class Minimax {
public:
    std::map<Piece, std::vector<int>> pieceScores;

    Minimax();
    float heuristicEval(Chess* chess);
    Evaluation searchABPruning(Chess* chess, int depth, float alpha = -INFINITE_EVAL, float beta = INFINITE_EVAL);
};

#endif // __MINIMAX__H__