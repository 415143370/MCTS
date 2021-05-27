#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include "aibotbase.h"

#include <list>
#include <QTime>
#include <QThread>
#include <QObject>

class RandomBot : public AiBotBase
{
    Q_OBJECT
public:
    RandomBot(GameState* gameState, const QTime* timeLeft);
    virtual ~RandomBot() override=default;

    void reset() override;
    void update(unsigned int moveIdx) override;

private:
    void selectBestMoves() override;
};

#endif // RANDOMBOT_H
