#include "randombot.h"

RandomBot::RandomBot(GameState* gameState, const QTime* timeLeft):
    AiBotBase(gameState, timeLeft)
{}

void RandomBot::selectBestMoves(){
    Color rootPlayer = gameState->getCurrentPlayer();
    Color currPlayer;
    do{
        gameState->update(gameState->getRandomMove());
        currPlayer = gameState->getCurrentPlayer();
    }while(rootPlayer == currPlayer);
}

void RandomBot::update(unsigned int moveIdx){
    // empty, gameState is expected to be updated at this point by the GUI
}

void RandomBot::reset(){
    // empty, gameState is expected to be reset at this point by the GUI
}
