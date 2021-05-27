#include "mast.h"
#include <math.h>

MAST::MAST(GameState* gameState, double temp, double w):
    gameState{gameState},
    temp{temp},
    w{w},
    moves{{}}
{
    scores = {vector<double>(gameState->moveNum(), 1.0), vector<double>(gameState->moveNum(), 1.0)};
}

void MAST::setup(){
    if(initialScores[WHITE].size() == 0 or initialScores[BLACK].size() == 0)
        initialScores = gameState->getInitialPolicy();
    scores = initialScores;
}

tuple<unsigned int, unsigned int> MAST::select() const{
    default_random_engine generator;
    Color currPlayer = gameState->getCurrentPlayer();
    list<int> probs;
    vector<unsigned int> idxMap;
    idxMap.reserve(gameState->validMoves.size());
    for(unsigned int moveIdx : gameState->validMoves){
        idxMap.push_back(moveIdx);
        // no normalization is needed, relative volume matters
        probs.push_back(exp(scores[currPlayer][moveIdx]/temp) + 1e-8);
    }

    discrete_distribution<> distribution (probs.begin(), probs.end());
    unsigned int idx = distribution(generator);
    return {idxMap[idx], idx};
}

void MAST::addMove(Color player, unsigned int moveIdx){
    moves.push_back({player, moveIdx});
}

void MAST::update(double outcome){
    for(Move& move : moves){
        // 1-outcome if black, faster then if block
        double val = outcome + move.player * (1.0-2.0*outcome);
        // update moving average
        scores[move.player][move.moveIdx] = w * scores[move.player][move.moveIdx] + (1 - w) * val;
    }
    moves.clear();
}

vector<double> MAST::getScores(Color playerColor) const{
    return scores[playerColor];
}

double MAST::getScore(unsigned int idx, Color playerColor) const{
    return scores[playerColor][idx];
}
