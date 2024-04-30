#include "LoserTree.h"
#include <iostream>

LoserTree::LoserTree(const std::vector<runs> &runList, Buffer *buffer) : _runList(runList), _buffer(buffer)
{
    k = runList.size();
    tree.resize(k);
    indices.resize(k, 0);

    for (int i = 0; i < k; ++i)
    {
        if (_runList.at(i).currentPageIndex <= HDD_PAGE_INDEX)
        {
            float latency = PAGE_SIZE / SSD_BANDWIDTH + SSD_LATENCY;
            latency *= 1000000;
            std::cout << "STATE -> READ_RUN_PAGES_SSD: Read sorted run pages from the SSD device" << std::endl;
            std::cout << "ACCESS -> A write to SSD was made with size " << PAGE_SIZE << " bytes and latency " << latency << "us" << std::endl;
        }
        else
        {
            float latency = PAGE_SIZE / HDD_BANDWIDTH + HDD_LATENCY;
            latency *= 1000000;
            std::cout << "STATE -> READ_RUN_PAGES_HDD: Read sorted run pages from the HDD device" << std::endl;
            std::cout << "ACCESS -> A write to HDD was made with size " << PAGE_SIZE << " bytes and latency " << latency << "us" << std::endl;
        }
        lists.push_back(buffer->getExistingPage(_runList.at(i).currentPageIndex)->getRecords());
        indices[i] = 0;
        _runList.at(i).currentPageIndex++;
    }
    initialize();
}

void LoserTree::initialize()
{
    for (int i = 0; i < k; ++i)
    {
        tree[i] = i;
    }
}

DataRecord LoserTree::next()
{
    if (tree.size() == 0)
    {
        return DataRecord();
    }

    make_heap(tree.begin(), tree.end(), [&](int a, int b)
              { return lists[a][indices[a]].key > lists[b][indices[b]].key; });

    int loser = tree.front();
    DataRecord record = lists[loser][indices[loser]];
    indices[loser]++;
    if (indices[loser] == lists[loser].size())
    {
        if (_runList.at(loser).currentPageIndex < _runList.at(loser).endPageIndex)
        {
            lists[loser] = _buffer->getExistingPage(_runList.at(loser).currentPageIndex)->getRecords();
            if (_runList.at(loser).currentPageIndex <= HDD_PAGE_INDEX)
            {
                size_t latency = PAGE_SIZE / SSD_BANDWIDTH + SSD_LATENCY;
                std::cout << "STATE -> READ_RUN_PAGES_SSD: Read sorted run pages from the SSD device" << std::endl;
                std::cout << "ACCESS -> A write to SSD was made with size" << PAGE_SIZE << "bytes and latency" << latency << "us" << std::endl;
            }
            else
            {
                size_t latency = PAGE_SIZE / HDD_BANDWIDTH + HDD_LATENCY;
                std::cout << "STATE -> READ_RUN_PAGES_HDD: Read sorted run pages from the HDD device" << std::endl;
                std::cout << "ACCESS -> A write to SSD was made with size" << PAGE_SIZE << "bytes and latency" << latency << "us" << std::endl;
            }
            indices[loser] = 0;
            _runList.at(loser).currentPageIndex++;
        }
        else
        {
            tree.erase(tree.begin());
        }
    }
    return record;
}