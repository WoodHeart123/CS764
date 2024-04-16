#pragma once

#include "defs.h"
#include <unordered_map>
#include "Disk.h"

class Buffer{
public:
    Buffer(size_t recordSize);
    // create a new page
    Page* createNewPage();
    // get an existing page, first from buffer, then from disk
    Page* getExistingPage(size_t pageIndex);
    size_t getTotalPages() const;
    // flush a page to disk
    bool flushPage(size_t pageIndex);
    // flush all pages to disk
    bool flushAllPages();
    // check if buffer is full
    bool isFull() const;
private:
    Disk* disk;
    std::unordered_map<size_t, Page> buffer;
    const static size_t bufferSize = BUFFER_SIZE;
    const static size_t numOfPagesInBuffer = BUFFER_SIZE / PAGE_SIZE;
    size_t totalPages;
    size_t recordSize;
};