#ifndef EVENSCHEDULER_H
#define EVENSCHEDULER_H

#include <QTime>
#include "gamestate.h"

class EvenScheduler
{
public:
    EvenScheduler(const QTime* timeLeft, GameState* gameState, unsigned int freq=100 , double reserveTime=2);
    virtual ~EvenScheduler()=default;

    bool finish();
    void schedule();
    void reset();

protected:
    // frequency of checking stop condition. Make sure that reserveTime is large enough to avoid running out of time
    unsigned int freq;
    double reserveTime;
    // elapsed time in milliseconds from the start of the current round
    unsigned int elapsedmsecs;
    // base time devoted for the current round
    unsigned int msecsBudget;
    // number of playouts since the beginnign of the current round
    double numPlayouts;
    QTime startTime;
    GameState* gameState;
    const QTime* timeLeft;
};

#endif // EVENSCHEDULER_H
