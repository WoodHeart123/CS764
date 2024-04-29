#pragma once

#include "defs.h"
#include <map>
#include <memory>
#include "Disk.h"

class Buffer
{
public:
    Buffer(size_t recordSize);
    ~Buffer();
    // create a new page
    std::shared_ptr<Page> createNewPage();
    // get an existing page, first from buffer, then from disk
    std::shared_ptr<Page> getExistingPage(size_t pageIndex);
    // get a sequence of pages, first from buffer, then from disk
    std::vector<std::shared_ptr<Page>> getExistingPages(size_t startPageIndex, size_t endPageIndex);
    size_t getTotalPages() const;
    void setTotalPages(size_t totalPages);
    // flush a page to disk
    bool flushPage(size_t pageIndex);
    // replace a page in buffer with a new page
    bool replacePage(size_t pageIndex, std::shared_ptr<Page> newPage);
    // erase a page from buffer
    bool erasePage(size_t pageIndex);
    // flush all pages to disk
    bool flushAllPages();
    // check if buffer is full
    bool isFull() const;
    // clear the buffer
    bool clear();
    bool removePage(size_t pageIndex);

    const static size_t bufferSize = BUFFER_SIZE;
    const static size_t numOfPagesInBuffer = BUFFER_SIZE / PAGE_SIZE;
private:
    Disk *disk;
    std::map<size_t, std::shared_ptr<Page>> buffer;
    size_t totalPages;
    size_t _recordSize;
};