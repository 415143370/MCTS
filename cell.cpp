#include "cell.h"

Cell::Cell(int q, int r, unsigned int idx):
    q{q},
    r{r},
    idx{idx},
    color{Color::EMPTY},
    groupId{-1}
{}

Group::Group(const unsigned int id, const unsigned int newGroupSize):
    id{id},
    size{newGroupSize}
{}

unsigned int Cell::findSuperGroup(unsigned int id, const vector<Group>& groups) const
{
    while(true)
    {
        // trace back the group id of the connected cells
        unsigned int superId = groups[id].id;
        if(id == superId) return superId;
        id = superId;
    }
}

list<unsigned int> Cell::getNeighbourGroupIds(const vector<Group>& groups, Color color) const
{
    list<unsigned int> neighbourSuperGroups;
    auto it = neighbours.begin();
    unsigned int nGroupId;
    unsigned int firstNGroupId = -1;
    unsigned int counter = 0;
    // we use a counter instead of comparing with the iterator of end() because step size can be 2
    while(counter < neighbours.size()){
        if((*it)->color == color){
            nGroupId = findSuperGroup((*it)->groupId, groups);
            // first valid group that is connected
            if(firstNGroupId == -1){
                firstNGroupId = nGroupId;
                neighbourSuperGroups.push_back(firstNGroupId);
            }
            // second group should have different id
            else if(firstNGroupId != nGroupId){
                neighbourSuperGroups.push_back(nGroupId);
                counter += 2;
                std::advance(it, 2); // skip the next one as they would have the same group id
                break;
            }
            ++counter;
            ++it; // skip the next one as they would have the same group id
        }
        ++counter;
        ++it;
    }
    unsigned int thirdNGroupId;
    // the third one should be different from the previous 2 groups
    while(counter < neighbours.size()){
        if((*it)->color == color){
            thirdNGroupId = findSuperGroup((*it)->groupId, groups);
            if(thirdNGroupId != nGroupId and thirdNGroupId != firstNGroupId){
                neighbourSuperGroups.push_back(thirdNGroupId);
                return neighbourSuperGroups; // there are maximum 3 distinct groups
            }
        }
        ++counter;
        ++it;
    }

    return neighbourSuperGroups;
}
