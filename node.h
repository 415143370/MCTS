#ifndef NODE_H
#define NODE_H

// forward declarations
template<typename T>
class Node;

template<typename T>
class StopScheduler;

template<typename T>
class RecyclingNode;

// type_traits for friend declarations
template<typename T>
struct WType{
    typedef T type;
};

template<typename T>
struct WType<RecyclingNode<T>>{
    typedef T type;
};

#include "mast.h"
#include "zhashtable.h"

template<typename T>
class Node
{
    // private static interface
    friend T;
    friend class WType<T>::type;
    friend class ZHashTable<T>;
    friend class StopScheduler<T>;

    // there is no partial specialization for friend declaration
    template<typename X, typename Y, typename Z>
    friend class MCTS;

    // static interface, no instances
    Node()=delete;
    ~Node()=delete;
    Node(const Node&)=delete;
    Node& operator=(const Node&)=delete;
    static void setup(MAST* policy, GameState* gameState, ZHashTable<T>* tTable);

    // mcts updates
    static T* selectMostVisited();
    static T* expand();
    static void backward();
    static void backprop(double outcome);

    static void manageMemory();

    inline static GameState* gameState;
    inline static ZHashTable<T>* tTable;
    inline static MAST* policy;

    // node to remove. Deallocation is postponed after backpropagation to avoid deleting a node from the path
    inline static T* rNode;

    inline static unsigned int currDepth;
};

template<typename T>
void Node<T>::setup(MAST* policy, GameState* gameState, ZHashTable<T>* tTable)
{
    Node<T>::policy = policy;
    Node<T>::gameState = gameState;
    Node<T>::tTable = tTable;
}

template<typename T>
T* Node<T>::selectMostVisited(){
    double maxVisit = -1;
    unsigned int bestMoveIdx;
    T* bestChild;
    for(unsigned int moveIdx : Node<T>::gameState->validMoves){
        Node<T>::tTable->update(moveIdx);
        T* child = Node<T>::tTable->load();
        double visit = child ? child->visitCount() : 0;
        if(visit > maxVisit){
            maxVisit = visit;
            bestChild = child;
            bestMoveIdx = moveIdx;
        }
        // xor twice with the same value gives back the original
        Node<T>::tTable->update(moveIdx);
    }
    Node<T>::gameState->update(bestMoveIdx);
    Node<T>::tTable->update(bestMoveIdx);
    return bestChild;
}

template<typename T>
void Node<T>::backward(){
    unsigned int moveIdx = Node<T>::gameState->takenMove();
    Node<T>::gameState->undo();
    Node<T>::tTable->update(moveIdx);
}

template<typename T>
T* Node<T>::expand(){
    return Node<T>::tTable->store();
}

template<typename T>
void Node<T>::manageMemory(){
    delete Node<T>::rNode;
    Node<T>::rNode = nullptr;
}

#endif // NODE_H
