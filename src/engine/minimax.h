#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include <iterator>
#include <cstdint>
#include <map>
#include <memory>

#include "../chess/chess.h"

const float INFINITE_EVAL = 10000.0f;

typedef struct Evaluation {
    float result;
    Move move;
} Evaluation;

typedef struct FinalEvaluation {
    float result;
    Move move;
    int steps;
    long long heuristicTime;
    long long moveGenTime;
} FinalEvaluation;

class Minimax {
public:
    int pieceScores[15][64] = {0};
    int steps = 0;
    long long heuristicTime = 0;

    Minimax();
    float heuristicEval(std::shared_ptr<Chess> chess);
    float quiescenceSearch(std::shared_ptr<Chess> chess, float alpha, float beta, int depth);
    FinalEvaluation searchABPruning(Chess chess, int depth);
    Evaluation searchABPruningExec(std::shared_ptr<Chess> chess, int depth, float alpha, float beta);
};

#endif // __MINIMAX__H__