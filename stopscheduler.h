#ifndef STOPSCHEDULER_H
#define STOPSCHEDULER_H

#include "zhashtable.h"
#include "stopscheduler.h"
#include <cassert>
#include <math.h>

#define assertm(exp, msg) assert(((void)msg, exp))

template<typename T>
class StopScheduler{
public:
    StopScheduler(const QTime* timeLeft,
                  GameState* gameState,
                  ZHashTable<T>* tTable,
                  double p=0.9,
                  unsigned int freq=100,
                  double reserveTime=1);
    virtual ~StopScheduler()=default;

    bool finish();
    void schedule();
    void reset() {}

protected:
    // p * msecsBudget time is given to the second best child to catch up
    const double p;
    // frequency of checking stop condition. Make sure that reserveTime is large enough to avoid running out of time
    const unsigned int freq;
    double reserveTime;
    // round per milliseconds
    double speed;
    // elapsed time in milliseconds from the start of the current round
    unsigned int elapsedmsecs;
    // base time devoted for the current round
    unsigned int msecsBudget;
    // number of playouts since the beginnign of the current round
    double numPlayouts;
    QTime startTime;
    double n;

    // parabolic parameters
    double a;
    double b;
    double c;
    double w;
    const QTime* timeLeft;
    GameState* gameState;
    ZHashTable<T>* tTable;
};

template<typename T>
StopScheduler<T>::StopScheduler(const QTime* timeLeft,
                                       GameState* gameState,
                                       ZHashTable<T>* tTable,
                                       double p,
                                       unsigned int freq,
                                       double reserveTime):
    timeLeft{timeLeft},
    gameState{gameState},
    tTable{tTable},
    p{p},
    freq{freq},
    reserveTime{reserveTime}
{
    assertm((p >= 0 or p<=1), "p argument should be greater than 0 and smaller or equal to 1");
    assertm(reserveTime > 0, "reserveTime argument should be at least 0");
    assertm(freq >= 2, "freq argument should be at least 2");
    // compute parabolic time distrubution: m is for the middle and s is for the starting move
    // we are fitting a parabolic curve to 3 points: (x1,y1), (x2,y2), (x3,y3)
    n = gameState->numExpectedMoves();
    double m = 1.0 + (n/2.0-1.0)/2;
    double s = 1.0;
    double x1 = 1.0;
    double y1 = 1.0;
    double x2 = (1.0+n)/2.0;
    double y2 = m;
    double x3 = n;
    double y3 = s;

    double denom = (x1 - x2)*(x1 - x3)*(x2 - x3);
    a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
    b = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denom;
    c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

    // check if slope of parabola is under the y=x line so we can not run out of time
    assertm(2*a+b < 1, "invalid parabolic curve, lower the value of m");
    // m is assumed to be greater than s: we use time budget
    assertm(s < m, "invalid parabolic curve, s should be smaller than m");
    // m and s should be greater than 0
    assertm(s > 0.0 and m > 0.0, "invalid parabolic curve, s and m should be greater than 0");
}

template<typename T>
bool StopScheduler<T>::finish(){
    ++numPlayouts;
    // Make sure that reserve time is large enough to run full cycles at least frequency times otherwise
    // it is not quaranteed that the AI not runs out of time
    if(fmod(numPlayouts+1, freq) != 0.0)
        return false;
    elapsedmsecs = (*timeLeft).msecsTo(startTime);
    // if the time spent for current search is more than the budget
    if(msecsBudget <= elapsedmsecs)
        return true;
    speed = numPlayouts / elapsedmsecs;
    double maxScore;
    double secondMaxScore;
    double score;
    // we only use the statescores for the computation
    T* node;
    T* bestNode;
    T* secondBestNode;
    maxScore = -1;
    secondMaxScore = -1;
    for(unsigned int moveIdx : gameState->validMoves){
        Node<T>::tTable->update(moveIdx);
        node = Node<T>::tTable->load();
        score = node ? node->visitCount() : 0;
        if(score > maxScore){
            secondMaxScore = maxScore;
            maxScore = score;
            secondBestNode = bestNode;
            bestNode = node;
        }
        else if(score > secondMaxScore){
            secondMaxScore = score;
            secondBestNode = node;
        }
        Node<T>::tTable->update(moveIdx);
    }
    // most likely there is no way for the AI to win
    if(bestNode->stateScore() < 0.01 and elapsedmsecs >= 500){
        return true;
    }
    // most likely the AI won
    if(bestNode->stateScore() > 0.99 and elapsedmsecs >= 500){
        return true;
    }
    // check if the best node can change within the dedicated time frame
    // estimate  of minimum number of playouts to change the best node (with regard to the visit count)
    double minPlayouts = maxScore - secondMaxScore;
    // check if the expected number of playouts that can be carried out within the dedicated time frame is smaller
    if(minPlayouts > p / w * speed * (msecsBudget - elapsedmsecs)){
        return true;
    }
    return false;
}

template<typename T>
void StopScheduler<T>::schedule(){
    numPlayouts = -1;
    startTime = *timeLeft;
    int rmsecs = QTime(0, 0, 0, 0).msecsTo(startTime) - reserveTime * 1000;
    n = gameState->numExpectedMoves();
    w  = (a*n*n + b*n + c);
    msecsBudget = w / n * (rmsecs > 0 ? rmsecs : 1);
}

#endif // STOPSCHEDULER_H
