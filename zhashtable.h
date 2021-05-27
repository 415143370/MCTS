#ifndef ZHASHTABLE_H
#define ZHASHTABLE_H

#include <vector>
#include <list>
#include <utility>

template<typename T>
class ZHashTable;

#include "recyclingnode.h"

#include <random>
#include <limits>

// type_traits to get wrapped node type
template<typename T>
struct isRecycled{
    static const bool value{false};
    typedef T wtype;
};

template<typename T>
struct isRecycled<RecyclingNode<T>>{
    static const bool value{true};
    typedef T wtype;
};

class ZHashTableBase{};

template<typename T>
class ZHashTable: ZHashTableBase
{
    // there is no partial specialization for friend declaration
    template<typename X, typename Y, typename Z>
    friend class MCTS;
public:
    ZHashTable(GameState* gameState, MAST* policy, unsigned int LenHashCode=20, unsigned int budget=50000);

    void reset();

    ~ZHashTable()=default;

    ZHashTable(const ZHashTable&)=delete;
    ZHashTable& operator=(const ZHashTable&)=delete;

    void update(unsigned int moveIdx);
    T* updateRoot(unsigned int moveIdx);
    T* load();
    T* store();

    typedef typename isRecycled<T>::wtype wType;
    static constexpr bool isRecycledType = isRecycled<T>::value;

protected:
    unsigned int LenHashCode;
    unsigned long int hashCodeMask;
    vector<list<T*>> table;
    vector<unsigned long int> hashCodes;
    vector<unsigned long int> hashKeys;
    unsigned long int currCode;
    unsigned long int currKey;
    // root node
    T* root;
};

// We could make constructor parameters dependent on the template type but the gains would be negligible
template<typename T>
ZHashTable<T>::ZHashTable(GameState* gameState, MAST* policy, unsigned int LenHashCode, unsigned int budget):
    LenHashCode{LenHashCode},
    currCode{0},
    currKey{0},
    table{vector<list<T*>>(pow(2, LenHashCode), list<T*>())}
{
    unsigned int moveNum = gameState->moveNum();
    if constexpr(isRecycledType){
        wType::template setup<T>(policy, gameState, this);
        T::setup(budget);
        root = store();
        T::fifo.push_back(root);
        root->fifoPtr = T::fifo.end();
        --(root->fifoPtr);
    }
    else{
        T::setup(policy, gameState, this);
        // root is not in TT
        root = new T(currKey);
    }

    hashCodes.reserve(moveNum);
    hashKeys.reserve(moveNum);
    // Mask to unset the most signifficant bits
    hashCodeMask=(1L<<LenHashCode)-1;

    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<unsigned long int> distr;

    for(unsigned int i=0; i<moveNum; ++i)
    {
        hashCodes.push_back(distr(eng));
        // set the most significant bits to zero (masking) to have correct index mapping
        hashCodes[i]&=hashCodeMask;
        hashKeys.push_back(distr(eng));
    }
}

template<typename T>
void ZHashTable<T>::reset(){
    for(auto& nodes : table){
        for(T* p : nodes){
            delete p;
        }
        nodes = list<T*>();
    }
    // reset static variables
    Node<T>::currDepth = currCode = currKey = 0;
    T::reset();

    if constexpr(isRecycledType){
        wType::reset();
        root = store();
        T::fifo.push_back(root);
        root->fifoPtr = T::fifo.end();
        --(root->fifoPtr);
    }
    else{
        // root is not in TT
        delete root;
        root = new T(currKey);
    }
}

template<typename T>
void ZHashTable<T>::update(unsigned int moveIdx)
{
    currCode^=hashCodes[moveIdx];
    currKey^=hashKeys[moveIdx];
}

template<typename T>
T* ZHashTable<T>::load()
{
    for(auto p : table[currCode]){
        if(p->key == currKey)
            return p;
    }
    return nullptr;
}

template<typename T>
T* ZHashTable<T>::store()
{
    // node recycling
    if constexpr(isRecycledType){
        table[currCode].push_front(new T(currKey));
        auto it = table[currCode].begin();
        // provide iterators so RecyclingNode can deallocate itself
        (*it)->listIt = it;
        (*it)->listPtr = &(table[currCode]);
        return *it;
    }
    else{
        // OneDepthVNew replacing scheme
        if(table[currCode].size() == 2){
            // reachable ? -> closer to root ? -> visit count ?
            // node deallocation is postponed after backpropagation
            if(table[currCode].front()->depth <= root->depth){
                Node<T>::rNode = table[currCode].front();
                table[currCode].pop_front();
            }
            else if(table[currCode].back()->depth <= root->depth){
                Node<T>::rNode = table[currCode].back();
                table[currCode].pop_back();
            }
            else if(table[currCode].front()->depth > table[currCode].back()->depth){
                Node<T>::rNode = table[currCode].front();
                table[currCode].pop_front();
            }
            else if(table[currCode].front()->depth < table[currCode].back()->depth){
                Node<T>::rNode = table[currCode].back();
                table[currCode].pop_back();
            }
            else if(table[currCode].front()->visitCount() < table[currCode].back()->visitCount()){
                Node<T>::rNode = table[currCode].front();
                table[currCode].pop_front();
            }
            else{
                Node<T>::rNode = table[currCode].back();
                table[currCode].pop_back();
            }
            table[currCode].push_back(new T(currKey));
        }
        else{
            table[currCode].push_back(new T(currKey));
        }
        return *(table[currCode].begin());
    }
}

template<typename T>
T* ZHashTable<T>::updateRoot(unsigned int moveIdx){
    update(moveIdx);
    if constexpr(isRecycledType){
        (root->listPtr)->erase(root->listIt);
        T::fifo.erase(root->fifoPtr);
    }
    delete root;
    ++Node<T>::currDepth;
    root = load();
    if constexpr(isRecycledType){
        // no copy is needed, root is in the TT
        if(!root){
            root = store();
            T::fifo.push_back(root);
            root->fifoPtr = T::fifo.end();
            --(root->fifoPtr);
        }
        return root;
    }
    else{
        // copy, root is not stored in TT
        root = root ? new T(*root) : new T(currKey);
        return root;
    }
}

#endif // ZHASHTABLE_H
