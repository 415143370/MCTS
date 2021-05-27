#include "evenscheduler.h"
#include <cassert>
#include <math.h>
#define assertm(exp, msg) assert(((void)msg, exp))

EvenScheduler::EvenScheduler(const QTime* timeLeft, GameState* gameState, unsigned int freq , double reserveTime):
    timeLeft{timeLeft},
    gameState{gameState},
    freq{freq},
    reserveTime{reserveTime}
{
    assertm(reserveTime > 0, "reserveTime argument should be at least 0");
    assertm(freq >= 2, "freq argument should be at least 2");
}

bool EvenScheduler::finish(){
    ++numPlayouts;
    if(fmod(numPlayouts+1, freq) != 0.0)
        return false;
    elapsedmsecs = (*timeLeft).msecsTo(startTime);

    // if the time spent for current search is more than the budget
    if(msecsBudget <= elapsedmsecs)
        return true;
    return false;
}

void EvenScheduler::schedule(){
    numPlayouts = -1;
    startTime = *timeLeft;
    unsigned int rmsecs = QTime(0, 0, 0, 0).msecsTo(startTime) - reserveTime * 1000;
    double numMoves = gameState->numExpectedMoves();
    msecsBudget = rmsecs / numMoves;
}

void EvenScheduler::reset(){
    //empty
}
