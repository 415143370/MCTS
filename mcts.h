#ifndef MCTS_H
#define MCTS_H

#include <stack>
#include "node.h"
#include "stopscheduler.h"

// Base class to prevent template spreading
class MCTSBase{
public:
    MCTSBase()=default;
    virtual ~MCTSBase()=default;
    virtual void reset()=0;
    virtual void run()=0;
    virtual void updateRoot(unsigned int moveIdx)=0;

};

template<typename NodeType, typename PolicyType=MAST, typename SchedulerType=StopScheduler<NodeType>>
class MCTS: public MCTSBase
{
public:
    MCTS(ZHashTable<NodeType>* tTable, GameState* gameState, PolicyType* policy, SchedulerType* scheduler):
        tTable{tTable},
        root{tTable->root},
        currNode{root},
        gameState{gameState},
        policy{policy},
        scheduler{scheduler},
        path{}
    {}

    virtual ~MCTS()=default;

    virtual void reset() override{
        policy->setup();
        tTable->reset();
        scheduler->reset();
        this->root = tTable->root;
        path = stack<NodeType*>();
    }

    virtual void updateRoot(unsigned int moveIdx) final{
        // gameState is expected to be updated
        root = tTable->updateRoot(moveIdx);
    }

    virtual void run() override{
        scheduler->schedule();
        while(!scheduler->finish()){
            selection();
            double outcome = simulation();
            backpropagation(outcome);
        }
        Color rootPlayer = gameState->getCurrentPlayer();
        do{
            NodeType* bestChild = root->selectMostVisited();
            // with TT it could be that there was only one child explored and removed
            ++Node<NodeType>::currDepth;
            if(bestChild)
                root = bestChild;
            else
                root = root->expand();
            currPlayer = gameState->getCurrentPlayer();
        }while(rootPlayer == currPlayer);
    }
protected:
    void selection(){
        currPlayer = gameState->getCurrentPlayer();
        // node selection updates gamestate and TT
        currNode = root;
        NodeType* child = root->select();
        ++Node<NodeType>::currDepth;
        policy->addMove(currPlayer, gameState->takenMove());
        while(!gameState->end() and child){
            currNode = child;
            path.push(currNode);
            currPlayer = gameState->getCurrentPlayer();
            child = currNode->select();
            ++Node<NodeType>::currDepth;
            // currPlayer here is the player who placed the last piece
            policy->addMove(currPlayer, gameState->takenMove());
        }
        // expansion, only expand non-terminal node
        if(!gameState->end()){
            currNode = currNode->expand();
            path.push(currNode);
            // move is added during selection
            auto [moveIdx, childIdx] = policy->select();
            // depending on the node type we may wish to update the leaf node with the simulated action
            currNode->updateLeaf(moveIdx, childIdx);
            tTable->update(moveIdx);
            currPlayer = gameState->getCurrentPlayer();
            gameState->update(moveIdx);
            currNode = tTable->load();
        }
    }

    double simulation(){
        double outcome;
        unsigned int numSim = 1;
        while(true){
            // terminal node
            if(gameState->end()){
                // white: 1 black: 0 draw 0.5
                outcome = gameState->getScore();
                policy->addMove(currPlayer, gameState->takenMove());
                break;
            }
            // if the simulated node is in TT, we stop simulation and backprop the stored value
            else if(currNode){
                outcome = currNode->stateScore();
                // white: score black: 1-score
                outcome = outcome + currPlayer * (1-2*outcome);
                break;
            }
            // keep on simulating
            else{
                // move is added during selection
                auto [moveIdx, childIdx] = policy->select();
                policy->addMove(currPlayer, gameState->takenMove());
                currPlayer = gameState->getCurrentPlayer();
                tTable->update(moveIdx);
                gameState->update(moveIdx);
                currNode = tTable->load();
                ++numSim;
            }
        }
        policy->update(outcome);
        // backward gamestate, transposition table and optionally collect additional data from simulation depending on the type of the node
        while(numSim > 0){
            // backward operates only on static members but we need an instance for polymorfism
            root->backward();
            --numSim;
        }
        return outcome;
    }

    void backpropagation(double outcome){
        while(!path.empty()){
            path.top()->backprop(outcome);
            path.pop();
            --Node<NodeType>::currDepth;
        }
        root->backpropRoot(outcome);
        root->manageMemory();
    }

    Color currPlayer;
    NodeType* root;
    NodeType* currNode;
    ZHashTable<NodeType>* tTable;
    GameState* gameState;
    PolicyType* policy;
    SchedulerType* scheduler;
    stack<NodeType*> path;
};

#endif // MCTS_H
