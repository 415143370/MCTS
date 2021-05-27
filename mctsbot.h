#ifndef MCTSBOT_H
#define MCTSBOT_H

#include <QTime>


#include "mast.h"
#include "aibotbase.h"

#include "stopscheduler.h"
#include "evenscheduler.h"

#include "hmcravenode.h"
#include "uctnode.h"
#include "mcts.h"

class MCTSBot: public AiBotBase
{
    Q_OBJECT
public:
    MCTSBot(GameState* gameState, const QTime* timeLeft, QString node, bool recycling, unsigned int budget);
    virtual ~MCTSBot() override;
    virtual void reset() override;
    virtual void update(unsigned int moveIdx) override;
    virtual void setup() override;

private:
    void selectBestMoves() override;
    MCTSBase* mcts;
    MAST* policy;
};

#endif // MCTSBOT_H
