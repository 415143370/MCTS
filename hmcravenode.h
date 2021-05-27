#ifndef RAVENode_H
#define RAVENode_H

#include "recyclingnode.h"
#include "zhashtable.h"
#include "mast.h"
#include <array>
#include <list>

class RAVENode
{
    friend class ZHashTable<RAVENode>;
    friend class RecyclingNode<RAVENode>;
    friend class ZHashTable<RecyclingNode<RAVENode>>;
    friend class Node<RAVENode>;

public:
    RAVENode(const RAVENode&)=default;
    RAVENode& operator=(const RAVENode&)=default;

    template<typename T=RAVENode>
    inline T* select();

    template<typename T=RAVENode>
    inline T* selectMostVisited();

    template<typename T=RAVENode>
    inline T* expand();

    inline void updateLeaf(unsigned int moveIdx, unsigned int childIdx){}

    template<typename T=RAVENode>
    inline void backward();

    template<typename T=RAVENode>
    inline void backprop(double outcome);

    template<typename T=RAVENode>
    inline void backpropRoot(double outcome);

    template<typename T=RAVENode>
    inline void manageMemory();

    template<typename T=RAVENode>
    inline double actionScore(RAVENode* child, unsigned int moveIdx, Color playerColor) const;

    inline double stateScore() const;
    inline double visitCount() const;

    const unsigned long int key;
    const unsigned int depth;

protected:
    template<typename T=RAVENode>
    RAVENode(unsigned long int key, const T* =nullptr);

    virtual ~RAVENode()=default;

    template<typename T=RAVENode>
    static void setup(MAST* policy, GameState* gameState, ZHashTable<T>* tTable);

    inline static void reset();

    inline void updateMC(double val);
    inline void updateRAVE(double val, Color player, Color piece);

    // k value for weigthing MC and AMAF values
    static constexpr double k = 500;

    // MC values are stored at the child nodes so they get more samples
    double mcMean;
    double mcCount;

    // AMAF values are stored at the parent, we could use a vector but that might need a lot more memory (should be the length of all possible moves)
    vector<double> rMean;
    vector<double> rCount;
    // moves to update during backpropagation: playercolor-piececolor-moveidx
    inline static array<array<list<unsigned int>, 2>, 2> takenMoves;
};

template<typename T>
RAVENode::RAVENode(unsigned long int key, const T*):
    key{key},
    depth{Node<T>::currDepth},
    mcCount{1}
{
    mcMean = depth > 0 ? Node<T>::policy->getScore(Node<T>::gameState->takenMove(), Node<T>::gameState->getPreviousPlayer()) : 0.5;
    // assign initial values from the default policy (heuristical assignment)
    rMean = Node<T>::policy->getScores(Node<T>::gameState->getCurrentPlayer());
    // confidence is given by the number of equivalent samples
    rCount = vector<double>(rMean.size(), 1);
}

template<typename T>
void RAVENode::setup(MAST* policy, GameState* gameState, ZHashTable<T>* tTable)
{
    Node<T>::setup(policy, gameState, tTable);
    RAVENode::takenMoves = {};
}

void RAVENode::reset(){
    RAVENode::takenMoves = {};
}

template<typename T>
T* RAVENode::select(){
    double maxScore = -1;
    double score;
    unsigned int bestMoveIdx;
    T* bestChild;
    Color playerColor = Node<T>::gameState->getCurrentPlayer();
    for(unsigned int moveIdx : Node<T>::gameState->validMoves){
        Node<T>::tTable->update(moveIdx);
        T* child = Node<T>::tTable->load();
        score = actionScore<T>(child, moveIdx, playerColor);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
        }
        // xor twice with the same value gives back the original
        Node<T>::tTable->update(moveIdx);
    }
    // visit the node
    Node<T>::gameState->update(bestMoveIdx);
    Node<T>::tTable->update(bestMoveIdx);
    return bestChild;
}

void RAVENode::updateMC(double val){
    // MC values are stored at child nodes to have more samples
    mcMean = (mcMean*mcCount+val)/(mcCount+1);
    ++mcCount;
}

void RAVENode::updateRAVE(double val, Color player, Color piece){
    for(unsigned int moveIdx : RAVENode::takenMoves[player][piece]){
        rMean[moveIdx] = (rMean[moveIdx] * rCount[moveIdx]+val)/(rCount[moveIdx]+1);
        ++rCount[moveIdx];
    }
}

template<typename T>
void RAVENode::backprop(double outcome){
    Color player = Node<T>::gameState->getCurrentPlayer();
    Color piece = Node<T>::gameState->getCurrentColor();
    // action value is updated with the current player
    updateRAVE(outcome+player*(1.0-2.0*outcome), player, piece);
    unsigned int moveIdx = Node<T>::gameState->takenMove();
    Node<T>::gameState->undo();
    // state value is updated with parent player
    player = Node<T>::gameState->getCurrentPlayer();
    updateMC(outcome+player*(1.0-2.0*outcome));
    piece = Node<T>::gameState->getCurrentColor();
    RAVENode::takenMoves[player][piece].push_back(moveIdx);
    Node<T>::tTable->update(moveIdx);
}

template<typename T>
void RAVENode::backpropRoot(double outcome){
    Color player = Node<T>::gameState->getCurrentPlayer();
    Color piece = Node<T>::gameState->getCurrentColor();
    // action value is updated with the current player
    updateRAVE(outcome+player*(1.0-2.0*outcome), player, piece);
    RAVENode::takenMoves = {};
}

double RAVENode::stateScore() const {
    return mcMean;
}

double RAVENode::visitCount() const {
    return mcCount;
}

template<typename T>
double RAVENode::actionScore(RAVENode* child, unsigned int moveIdx, Color playerColor) const {
    double beta = sqrt(RAVENode::k / ((child ? child->mcCount : 0) + RAVENode::k));
    double score = (1-beta) * (child ? child->mcMean : Node<T>::policy->getScore(moveIdx, playerColor)) + beta * (rMean[moveIdx]);
    return score;
}

template<typename T>
void RAVENode::backward(){
    unsigned int moveIdx = Node<T>::gameState->takenMove();
    Node<T>::gameState->undo();
    // player is the one who played the move
    Color player = Node<T>::gameState->getCurrentPlayer();
    Color piece = Node<T>::gameState->getCurrentColor();
    RAVENode::takenMoves[player][piece].push_back(moveIdx);
    Node<T>::tTable->update(moveIdx);
}

template<typename T>
T* RAVENode::selectMostVisited(){
    return Node<T>::selectMostVisited();
}

template<typename T>
T* RAVENode::expand(){
    return Node<T>::expand();
}

template<typename T>
void RAVENode::manageMemory(){
    Node<T>::manageMemory();
}

#endif // RAVENode_H
