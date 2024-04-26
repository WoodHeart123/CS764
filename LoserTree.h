#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "defs.h"  
#include "Data.h"

class LoserTree {
public:
    struct ExternalNode {
        const DataRecord* record;  // Pointer to the record
        size_t recordIndex;
        size_t pageIndex;             // Index of the page in its sequence
        size_t sequenceIndex;         // Index of the sequence
        bool isExhausted;          
    };

    LoserTree(const std::vector