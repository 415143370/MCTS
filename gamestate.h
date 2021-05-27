#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <cmath>
#include <map>
#include <array>

#include "cell.h"

struct Ax{
    int q, r;
    Ax(int q, int r): q{q}, r{r} {}
};

class GameState
{
public:
    enum FeatureFlags
    {
        // additional features that the gamestate should compute besides group sizes
        FreeNeighbours = 1,
        Separators = 2,
    };
    GameState(int boardSize, FeatureFlags flags);

    // ---- inline functions for both external and internal usage ----
    inline bool isFlagSet(FeatureFlags flag) const{
        return flags&static_cast<unsigned int>(flag);
    }

    unsigned int computeCellNum(unsigned int boardSize) const;

    void reset();

    // cellNum should be declared before freeCells!
    const unsigned int cellNum;
    Color getCurrentPlayer() const;
    Color getPreviousPlayer() const;
    Color getCurrentColor() const;
    bool end() const;
    Color leader();
    double getScore();
    void update(unsigned int moveIdx);
    void undo();
    map<Color, int> getPlayerScores() const;
    unsigned int takenMove() const;
    unsigned int numExpectedMoves() const;
    unsigned int getRandomMove() const;
    unsigned int getWhiteCell() const;
    unsigned int getBlackCell() const;
    unsigned int moveNum() const;
    unsigned int toMoveIdx(unsigned int cellIdx, unsigned int pieceIdx) const;
    unsigned int lastTakenCellIdx() const;
    array<vector<double>, 2> getInitialPolicy();

private:
    // ---- available moves ----
    class ValidMoves
    {
        // container class for storing the available cells
    public:
        ValidMoves(unsigned int cellNum);
        unsigned int prevCellIdx() const;
        void remove(unsigned int cellIdx);
        unsigned int getRandomMove() const;
        void undo();
        unsigned int size() const;

        struct FreeCell{
            unsigned int idx;
            FreeCell* next;
            FreeCell* prev;
            FreeCell(unsigned int idx);
        };

        struct Iterator
        {
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = unsigned int;
            using pointer           = FreeCell*;
            using reference         = unsigned int&;

            Iterator(pointer ptr, ValidMoves* parent) : m_ptr(ptr), parent{parent} {}

            value_type operator*() const { return (*m_ptr).idx + parent->cellNum * parent->color; }
            pointer operator->() { return m_ptr; }
            Iterator& operator++() { m_ptr=m_ptr->next; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; }
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }

        private:
            pointer m_ptr;
            ValidMoves* parent;
        };

        Iterator begin() { return Iterator(first, this); }
        Iterator end()   { return Iterator(nullptr, this); }

    public:
        // freeCells for forward iterating in randomly ordered cells
        vector<FreeCell> freeCells;
        // lookup for instant accessing items from freeCells
        vector<FreeCell*> lookup;
        FreeCell* first;
        stack<unsigned int> takenCells;
        unsigned int mSize;
        unsigned int cellNum;
        unsigned int color;
    };

    // ---- initialization ----
    void initCells();
    void setNeighbours(Cell& cell);

    // ---- forward update ----
    void mergeGroups(Cell& cell);
    void updateColors();
    void updateNeighbourBitMaps(Cell& cell);
    void mergeNeighbourBitMaps(Cell& cell);

    // ---- backward update ----
    void decomposeGroup(Cell& cell);
    void undoColors();
    void undoOppBitMaps(const Cell& cell);

    // ---- bit manipulations ----
    unsigned int popCnt64(uint64_t i);

    // --- inline functions for internal usage ---
    inline Cell& idxToCell(unsigned int idx);
    inline Cell& axToCell(Ax ax);
    inline unsigned int axToIdx(Ax ax) const;
    inline bool isValidAx(const Ax& ax);

    // ---- variables ----

    FeatureFlags flags;
    unsigned int numSteps;
    unsigned int freeNeighbourBitMapSize;
    const int boardSize;
    vector<vector<Cell>> cells;
    vector<Cell*> cellVec;

    map<Color, int> playerScores;
    size_t bitmapSize;
    Color currentColor;
    Color currentPlayer;
    Color previousPlayer;
    list<unsigned int> moveIdxs;
public:
    ValidMoves validMoves;
    map<Color, vector<Group>> groups;
};

#endif // GAMESTATE_H
