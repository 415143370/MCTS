#ifndef AIBOTBASE_H
#define AIBOTBASE_H

#include "gamestate.h"

#include <vector>
#include <QTime>
#include <QThread>
#include <QObject>

class AiBotBase : public QObject
{
    Q_OBJECT
    friend class BoardDialog;
public:
    AiBotBase(GameState* gameState, const QTime* timeLeft);
    ~AiBotBase()=default;
    void updateGame();

    virtual void reset()=0;

    AiBotBase(const AiBotBase&)=delete;
    AiBotBase& operator=(const AiBotBase&)=delete;

    virtual void update(unsigned int moveIdx)=0;

    virtual void setup() {}

signals:
    void finished();

private slots:
    void updateGameSlot();

protected:
    GameState* gameState;
    inline bool isTimeOut() const{
        return timeLeft->toString("m:ss") == "0:00";
    }
    const QTime* timeLeft;
    // function should specify whiteMove and blackMove
    virtual void selectBestMoves()=0;

private:
    QThread thread;
};

#endif // AIBOTBASE_H
