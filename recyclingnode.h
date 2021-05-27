#ifndef RT_H
#define RT_H

// forward declaration
template<typename T>
class ZHashTable;

template<typename T>
class Node;

template<typename X, typename Y, typename Z>
class MCTS;

#include "node.h"

#include<list>
#include<vector>
#include <deque>

using namespace std;

template<typename T>
class RecyclingNode: public T
/*
 * template class extending T with node recycling using policy based design
 */
{
    typedef RecyclingNode<T> RT;
    typedef Node<RT> NRT;
    // there is no partial specialization for friend declaration
    template<typename X, typename Y, typename Z>
    friend class MCTS;

    friend class ZHashTable<RT>;

public:

    RecyclingNode(const RecyclingNode&)=delete;
    RecyclingNode& operator=(const RT&)=delete;

    double stateScore() const{
        return T::stateScore();
    }

    double visitCount() const{
        return T::visitCount();
    }

protected:
    // internal memory management by zhashtable

    explicit RecyclingNode(unsigned long int key):
        // wrapped node should have a key member for TT, p is for type deduction only
        T{key, RT::p}
    {
        ++RT::numNodes;
    }

    virtual ~RecyclingNode()
    {
        --RT::numNodes;
    }

    static void setup(unsigned int budget){
        RT::budget = budget;
        RT::numNodes = 0;
    }

    static void reset(){
        RT::fifo.clear();
        RT::numNodes = 0;
    }

    void manageMemory(){
        // node recycling
        if(RT::numNodes >= RT::budget){
            // we could replace these to the destructor but that would confilct with the
            // hashtable's implementation
            RT* front = RT::fifo.front();
            // remove from fifo
            RT::fifo.pop_front();
            // remove from TT
            (front->listPtr)->erase(front->listIt);
            // deallocate node
            delete front;
        }
    }

    RT* select()
    {
        // we are a non-leaf node so remove from FIFO (and later push back during backpropagation)
        // erase through reverse iterator
        RT::fifo.erase(fifoPtr);
        return T::template select<RT>();
    }

    RT* selectMostVisited(){
        return NRT::selectMostVisited();
    }

    RT* expand(){
        return T::template expand<RT>();
    }

    void backprop(double outcome)
    {
        RT::fifo.push_back(this);
        fifoPtr = RT::fifo.end();
        --fifoPtr;
        T::template backprop<RT>(outcome);
    }

    void backpropRoot(double outcome){
        RT::fifo.push_back(this);
        fifoPtr = RT::fifo.end();
        --fifoPtr;
        T::template backpropRoot<RT>(outcome);
    }

    void backward(){
        T::template backward<RT>();
    }

    void updateLeaf(unsigned int moveIdx, unsigned int childIdx){
        T::updateLeaf(moveIdx, childIdx);
    }

    // number of available nodes
    inline static unsigned int budget;
    inline static unsigned int numNodes;

    inline static list<RT*> fifo;
    typename list<RT*>::iterator fifoPtr;

    // iterator and pointer to list in TT
    typename list<RT*>::iterator listIt;
    list<RT*>* listPtr;

    // pointer for type deduction in wrapped node constructor
    static constexpr RT* p=nullptr;
};

#endif // RT_H
