#include "mctsbot.h"
#include <cassert>
#define assertm(exp, msg) assert(((void)msg, exp))

MCTSBot::MCTSBot(GameState* gameState, const QTime* timeLeft, QString node, bool recycling, unsigned int budget):
    AiBotBase(gameState, timeLeft)
{
    policy = new MAST(gameState);
    if(recycling){
        if(node == "UCT-2"){
            auto tTable = new ZHashTable<RecyclingNode<UCTNode>>(gameState, policy, 20, budget);
            auto scheduler = new StopScheduler<RecyclingNode<UCTNode>>(timeLeft, gameState, tTable);
            mcts = new MCTS<RecyclingNode<UCTNode>>(tTable, gameState, policy, scheduler);
        }
        else if(node == "MCRAVE"){
            auto tTable = new ZHashTable<RecyclingNode<RAVENode>>(gameState, policy, 20, budget);
            auto scheduler = new StopScheduler<RecyclingNode<RAVENode>>(timeLeft, gameState, tTable);
            mcts = new MCTS<RecyclingNode<RAVENode>>(tTable, gameState, policy, scheduler);
        }
        else
            assertm(false, "Invalid node type");
    }
    else{
        if(node == "UCT-2"){
            auto tTable = new ZHashTable<UCTNode>(gameState, policy, 20);
            auto scheduler = new StopScheduler<UCTNode>(timeLeft, gameState, tTable);
            mcts = new MCTS<UCTNode>(tTable, gameState, policy, scheduler);
        }
        else if(node == "MCRAVE"){
            auto tTable = new ZHashTable<RAVENode>(gameState, policy, 20);
            auto scheduler = new StopScheduler<RAVENode>(timeLeft, gameState, tTable);
            mcts = new MCTS<RAVENode>(tTable, gameState, policy, scheduler);
        }
        else
            assertm(false, "Invalid node type");
    }
}

MCTSBot::~MCTSBot(){
    delete mcts;
    delete policy;
}

void MCTSBot::selectBestMoves(){
    mcts->run();
}

void MCTSBot::update(unsigned int moveIdx){
    mcts->updateRoot(moveIdx);
}

void MCTSBot::reset(){
    // gameState is expected to be reset at this point by the GUI
    mcts->reset();
}

void MCTSBot::setup(){
    // wasteful but marginal
    mcts->reset();
}
