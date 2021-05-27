#ifndef MAST_H
#define MAST_H

#include "gamestate.h"

#include <random>
#include <vector>
#include <array>
#include <list>
#include <tuple>

class MAST
{
public:
    MAST(GameState* gameState, double temp=5, double w=0.98);
    ~MAST()=default;
    MAST(const MAST&)=delete;
    MAST& operator=(const MAST&)=delete;
     tuple<unsigned int, unsigned int> select() const;
    void update(double outcome);
    void addMove(Color player, unsigned int moveIdx);
    void reset();
    void setup();
    vector<double> getScores(Color playerColor) const;
    // should be const specified but we want to use [] operator on scores member
    double getScore(unsigned int idx, Color playerColor) const;

protected:
    struct Move{
        Color player;
        unsigned int moveIdx;
        Move(Color player, unsigned int moveIdx): player{player}, moveIdx{moveIdx} {}
    };
    array<vector<double>, 2> scores;

    array<vector<double>, 2> initialScores;
    list<Move> moves;
    double w;
    double temp;
    GameState* gameState;
};

#endif // MAST_H
