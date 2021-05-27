#include "aibotbase.h"

AiBotBase::AiBotBase(GameState* gameState, const QTime* timeLeft):
    QObject(nullptr), // can not have a parent as it will be moved to QThread
    gameState{gameState},
    timeLeft{timeLeft}
{
    this->moveToThread(&thread);
    connect(&thread, SIGNAL (started()), this, SLOT (updateGameSlot()));
    connect(this, SIGNAL (finished()), &thread, SLOT (quit()));
}

void AiBotBase::updateGame(){
    thread.start();
}

void AiBotBase::updateGameSlot(){
    selectBestMoves();
    emit finished();
}
