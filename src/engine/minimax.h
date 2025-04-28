#ifndef __MINIMAX_H__
#define __MINIMAX_H__

#include <vector>
#include <iterator>
#include <cstdint>
#include <map>
#include <memory>

#include "../chess/chess.h"

const float INFINITE_EVAL = 1000.0f;

typedef struct {
    float result;
    Move move;
    int steps;
    long long heuristicTime;
    long long moveGenTime;
}Evaluation;

class Minimax {
public:
    std::map<Piece, std::vector<int>> pieceScores;

    Minimax();
    float heuristicEval(std::shared_ptr<Chess> chess, size_t totalMoves);
    Evaluation searchABPruning(Chess chess, int depth);
    Evaluation searchABPruningExec(std::shared_ptr<Chess> chess, int depth, float alpha, float beta, int& steps, long long& heuristicTime, long long& moveGenTime);
};

#endif // __MINIMAX__H__