#include "Buffer.h"
#include <vector>
#include <string>
#include <iostream>

Buffer::Buffer(size_t size) : totalPages(0), _recordSize(size)
{
    disk = new Disk();
}

Buffer::~Buffer()
{
    delete disk;
}

// Create a new page and add it to the buffer
std::shared_ptr<Page> Buffer::createNewPage()
{
    if(isFull()){
        clear();
    }
    size_t newPageIndex = totalPages;
    buffer[newPageIndex] = std::shared_ptr<Page>(new Page(_recordSize));
    buffer[newPageIndex]->setPageIndex(newPageIndex);
    totalPages++;
    return buffer[newPageIndex];
}

// Get an existing page from buffer or from disk if not in buffer
std::shared_ptr<Page> Buffer::getExistingPage(size_t pageIndex)
{
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        return buffer.at(pageIndex); // Page found in buffer
    }
    else
    {
        // Load page from disk if not found in buffer
        std::vector<std::shared_ptr<Page>> pages = disk->readPagesFromDisk(pageIndex * PAGE_SIZE, _recordSize, 1);
        if (!pages.empty())
        {
            // Assume only one page was read
            buffer[pageIndex] = pages[0];
            buffer[pageIndex]->setPageIndex(pageIndex);
            return buffer.at(pageIndex);
        }
    }
    return nullptr; // Page not found in buffer and could not be loaded from disk
}

// Get a sequence of pages from disk
std::vector<std::shared_ptr<Page>> Buffer::getExistingPages(size_t startPageIndex, size_t endPageIndex)
{
    std::vector<std::shared_ptr<Page>> diskPages = disk->readPagesFromDisk(startPageIndex * PAGE_SIZE, _recordSize, endPageIndex - startPageIndex + 1);
    size_t i = startPageIndex;
    for (auto &page : diskPages)
    {
        page->setPageIndex(i++);
        buffer[page->getPageIndex()] = page;
    }
    return diskPages;
}

// Get the total number of pages managed by the buffer
size_t Buffer::getTotalPages() const
{
    return totalPages;
}

bool Buffer::removePage(size_t pageIndex)
{
    buffer.erase(buffer.find(pageIndex));
    return true;
}

// Set the total number of pages managed by the buffer
void Buffer::setTotalPages(size_t totalPages)
{
    this->totalPages = totalPages;
}

bool Buffer::flushAllPages()
{
    TRACE(true);
    std::vector<std::shared_ptr<Page>> pages;
    pages.push_back(buffer.begin() -> second);
    size_t startIndex = buffer.begin() -> first;
    for (auto it = std::next(buffer.begin(), 1); it != buffer.end(); ++it) {
        if((it->first) - 1 == std::prev(it, 1)->first){
          pages.push_back(it->second);
        }else{
          disk->writePagesToDisk(startIndex * PAGE_SIZE, pages);
          startIndex = it->first;
          pages.push_back(it->second);
        }
    }
    printf("startIndex = %lu\n, size = %d\n", startIndex, pages.size());
    disk->writePagesToDisk(startIndex * PAGE_SIZE, pages);
    clear();
    return true;
}

// Check if the buffer is full
bool Buffer::isFull() const
{
    return buffer.size() >= numOfPagesInBuffer;
}

// Flush a specific page to disk
bool Buffer::flushPage(size_t pageIndex)
{
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        // Attempt to write the page to disk
        if (disk->writePagesToDisk(pageIndex * PAGE_SIZE, {it->second}))
        {
            // Remove the page from the buffer
            buffer.erase(it);
            return true;
        }
    }
    return false; // Page not found in buffer or flush failed
}

bool Buffer::erasePage(size_t pageIndex)
{
    buffer.erase(buffer.find(pageIndex));
    return true;
}

bool Buffer::clear()
{
    buffer.clear();
    return true;
}

// Replace a page in the buffer with a new page
bool Buffer::replacePage(size_t pageIndex, std::shared_ptr<Page> newPage)
{
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        buffer.erase(it);
    }
    it = buffer.find(newPage->getPageIndex());
    if (it != buffer.end())
    {
        buffer.erase(it);
    }
    newPage->setPageIndex(pageIndex);
    buffer[pageIndex] = newPage;
    return true;
}