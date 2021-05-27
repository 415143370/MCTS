#ifndef UCTNODE_H
#define UCTNODE_H

#include "recyclingnode.h"
#include <math.h>

#include <map>
#include <vector>

class UCTNode
{
    friend class ZHashTable<UCTNode>;
    friend class RecyclingNode<UCTNode>;
    friend class ZHashTable<RecyclingNode<UCTNode>>;
    friend class Node<UCTNode>;

public:
    template<typename T=UCTNode>
    inline T* select();

    template<typename T=UCTNode>
    inline T* selectMostVisited();

    template<typename T=UCTNode>
    inline T* expand();

    inline void updateLeaf(unsigned int moveIdx, unsigned int childIdx);

    template<typename T=UCTNode>
    inline void backprop(double outcome);

    template<typename T=UCTNode>
    inline void backpropRoot(double outcome) {}

    template<typename T=UCTNode>
    inline void backward();

    template<typename T=UCTNode>
    inline void manageMemory();

    const unsigned long int key;
    const unsigned int depth;

    inline double stateScore() const;
    inline double visitCount() const;

    template<typename T=UCTNode>
    inline double actionScore(UCTNode* child, unsigned int moveIdx, unsigned int childIdx, Color playerColor) const;

    // c value for balancing exploration and exploitation
    static constexpr double c = 2.0;
    static constexpr double initialvCount = 1.0;

protected:
    template<typename T=UCTNode>
    UCTNode(unsigned long int key, const T* =nullptr);

    template<typename T=UCTNode>
    inline static void setup(MAST* policy, GameState* gameState, ZHashTable<T>* tTable);

    inline static void reset() {}

    virtual ~UCTNode()=default;

    UCTNode(const UCTNode&)=default;
    UCTNode& operator=(const UCTNode&)=default;

    double mean;
    double vCount;
    vector<double> vCounts;
    inline static double logc;
};

template<typename T>
UCTNode::UCTNode(unsigned long int key, const T*):
    key{key},
    depth{Node<T>::currDepth},
    vCounts{}
{
    mean = depth > 0 ? Node<T>::policy->getScore(Node<T>::gameState->takenMove(), Node<T>::gameState->getPreviousPlayer()) : 0.5;
    unsigned int numChild = Node<T>::gameState->validMoves.size();
    auto iCount = UCTNode::initialvCount;
    vCount = iCount * numChild;
    vCounts = vector<double> (numChild, iCount);
}

template<typename T>
void UCTNode::setup(MAST* policy, GameState* gameState, ZHashTable<T>* tTable)
{
    Node<T>::setup(policy, gameState, tTable);
}

template<typename T>
T* UCTNode::select(){
    double maxScore = -1;
    double score;
    unsigned int bestMoveIdx;
    T* bestChild;
    unsigned int idx=0;
    unsigned int bestIdx;
    UCTNode::logc = UCTNode::c * log(vCount + 1);
    Color playerColor = Node<T>::gameState->getCurrentPlayer();
    for(unsigned int moveIdx : Node<T>::gameState->validMoves){
        Node<T>::tTable->update(moveIdx);
        T* child = Node<T>::tTable->load();
        score = actionScore<T>(child, moveIdx, idx, playerColor);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
            bestIdx = idx;
        }
        // xor twice with the same value gives back the original
        Node<T>::tTable->update(moveIdx);
        ++idx;
    }
    // update visit counts
    ++vCount;
    ++vCounts[bestIdx];
    // visit the node
    Node<T>::gameState->update(bestMoveIdx);
    Node<T>::tTable->update(bestMoveIdx);
    return bestChild;
}

template<typename T>
void UCTNode::backprop(double outcome){
    unsigned int moveIdx = Node<T>::gameState->takenMove();
    // currentPlayer is the next player to move. We use the player who played the move
    Node<T>::gameState->undo();
    double val = outcome+Node<T>::gameState->getCurrentPlayer()*(1.0-2.0*outcome);
    mean = (mean*(vCount-1)+val)/(vCount);
    Node<T>::tTable->update(moveIdx);
}

void UCTNode::updateLeaf(unsigned int moveIdx, unsigned int childIdx) {
    ++vCount;
    ++vCounts[childIdx];
}

double UCTNode::stateScore() const {
    return mean;
}

double UCTNode::visitCount() const {
    return vCount;
}

template<typename T>
double UCTNode::actionScore(UCTNode* child, unsigned int moveIdx, unsigned int childIdx, Color playerColor) const {
    return (child ? child->mean : Node<T>::policy->getScore(moveIdx, playerColor)) + sqrt(UCTNode::logc / vCounts[childIdx]);
}

template<typename T>
void UCTNode::backward(){
    return Node<T>::backward();
}

template<typename T=UCTNode>
T* UCTNode::selectMostVisited(){
    return Node<T>::selectMostVisited();
}

template<typename T=UCTNode>
T* UCTNode::expand(){
    return Node<T>::expand();
}

template<typename T=UCTNode>
void UCTNode::manageMemory(){
    Node<T>::manageMemory();
}

#endif // UCTNODE_H
