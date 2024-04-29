#pragma once
#include <vector>
#include <memory>
#include <queue>
#include "defs.h"  
#include "Data.h"
#include "Buffer.h"

struct runs{
	size_t startPageIndex;
	size_t endPageIndex;
  size_t currentPageIndex;
}; 

struct node{
  DataRecord record;
  size_t index;
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
    LoserTree(const std::vector<runs>& runList, Buffer* buffer);
    void initialize();
    DataRecord next();
};