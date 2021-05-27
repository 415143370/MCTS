#ifndef CELL_H
#define CELL_H

#include<list>
#include <stack>
#include <map>
#include <vector>

#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t

using namespace std;

enum Color{WHITE, BLACK, EMPTY};

struct Group
{
    Group(const unsigned int id, const unsigned int newGroupSize);
    // the number of cells in the group
    unsigned int size;
    // super group id
    unsigned int id;
    // keep track of the group ids that were connected to the new piece placed on the board
    map<Color, stack<list<unsigned int>>> addedGroupIds;
    // array of long values of indicating free or taken neighbours per bit of groups
    stack<vector<long int>> freeNeighbourBitMaps;
};

struct Cell{
    Cell(int q, int r, unsigned int idx);
    list<unsigned int> getNeighbourGroupIds(const vector<Group>& groups, Color color) const;
    unsigned int findSuperGroup(unsigned int id, const vector<Group>& groups) const;
    Color color;
    // axial coordinates of hexagons
    int q, r;
    // indices of hexagons (row-by-row from left to right from top to bottom)
    unsigned int idx;
    // list of neighbour cells. There is no destructor because the pointers do not have ownership
    list<Cell*> neighbours;
    // group index
    int groupId;
};

#endif // CELL_H
