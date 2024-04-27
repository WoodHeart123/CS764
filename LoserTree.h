#pragma once
#include <vector>
#include <memory>
#include "defs.h"  
#include "Data.h"
#include "Buffer.h"

struct runs{
	size_t startPageIndex;
	size_t endPageIndex;
    size_t currentPageIndex;
}; 


class LoserTree {
private:
    std::vector<int> tree;  // The loser tree
    std::vector<int> indices;  // Current indices of each list
    std::vector<std::vector<DataRecord>> lists;  // The sorted lists
    std::vector<runs> _runList;
    Buffer* _buffer;
    int k;  // Number of lists
    

public:
    LoserTree(const std::vector<runs>& runList, Buffer* buffer) : _runList(runList), _buffer(buffer) {
        k = lists.size();
        tree.resize(k, 0);
        indices.resize(k, 0);
        lists.resize(k);

        for (int i = 0; i < k; ++i) {
            lists.push_back(buffer -> getExistingPage(_runList.at(i).currentPageIndex) -> getRecords());
            indices[i] = 0;
            _runList.at(i).currentPageIndex++;
        }
        initialize();
    }

    void initialize() {
        for (int i = 0; i < k; ++i) {
            tree[i] = k; 
        }

        for (int i = k - 1; i >= 0; --i) {
            adjust(i);
        }
    }

    void adjust(int index) {
        int t = (k + index) / 2;  // Parent index in the tree
        while (t > 0) {
            if (index == k || (indices[tree[t]] != -1 && indices[index] != -1 && 
                lists[tree[t]][indices[tree[t]]] > lists[index][indices[index]])) {
                std::swap(tree[t], index);
            }
            t /= 2;
        }
        tree[0] = index;  // The winner
    }

    DataRecord* next() {
        if (indices[tree[0]] == -1) {
            return nullptr;
        }

        int winnerIndex = tree[0];
        DataRecord winnerRecord = lists[winnerIndex][indices[winnerIndex]];
        indices[winnerIndex]++;

        if (indices[winnerIndex] >= lists[winnerIndex].size()) {
            if(_runList.at(winnerIndex).currentPageIndex < _runList.at(winnerIndex).endPageIndex)
            {
                indices[winnerIndex] = 0;
                _runList.at(winnerIndex).currentPageIndex++;
                lists[winnerIndex] = _buffer -> getExistingPage(_runList.at(winnerIndex).currentPageIndex) -> getRecords();
            }
            indices[winnerIndex] = -1;  // Mark this list as exhausted
        }

        adjust(winnerIndex);
        return &winnerRecord;
    }
};