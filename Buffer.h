#pragma once

#include "defs.h"
#include <unordered_map>
#include "Disk.h"

class Buffer{
public:
    Buffer();
    // create a new page
    Page createNewPage();
    // get an existing page, first from buffer, then from disk
    Page getExistingPage(size_t pageIndex) const;
    // change an existing page, first from buffer, then read from disk and change
    bool setPage(size_t pageIndex, const Page& page);
    size_t getTotalPages() const;
    // flush a page to disk
    bool flushPage(size_t pageIndex);
private:
    Disk disk;
    std::unordered_map<size_t, Page> buffer;
    const static size_t bufferSize = BUFFER_SIZE;
    const static size_t numOfPagesInBuffer = BUFFER_SIZE / PAGE_SIZE;
    size_t totalPages;
};