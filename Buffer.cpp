#include "Buffer.h"
#include <vector>
#include <string>
#include <iostream>

Buffer::Buffer(size_t size) : totalPages(0), recordSize(size)
{
    buffer.reserve(numOfPagesInBuffer);
    disk = new Disk();
}

Buffer::~Buffer(){
  delete disk;
}

// Create a new page and add it to the buffer
std::shared_ptr<Page> Buffer::createNewPage()
{
    TRACE (true);
    if (buffer.size() >= numOfPagesInBuffer)
    {
        std::cerr << "Buffer is full, consider flushing pages to disk." << std::endl;
        return nullptr; // Buffer is full
    }
    size_t newPageIndex = totalPages;
    buffer[newPageIndex] = std::shared_ptr<Page> (new Page(recordSize));
    buffer[newPageIndex] -> setPageIndex(newPageIndex);
    totalPages++;
    return buffer[newPageIndex];
}

// Get an existing page from buffer or from disk if not in buffer
std::shared_ptr<Page> Buffer::getExistingPage(size_t pageIndex)
{
    TRACE (true);
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        return buffer.at(pageIndex); // Page found in buffer
    }
    else
    {
        // Load page from disk if not found in buffer
        std::vector<std::shared_ptr<Page>> pages = disk -> readPagesFromDisk(pageIndex * PAGE_SIZE, sizeof(DataRecord), 1);
        if (!pages.empty())
        {
            // Assume only one page was read
            buffer[pageIndex] = pages[0];
            return buffer.at(pageIndex);
        }
    }
    return nullptr; // Page not found in buffer and could not be loaded from disk
}

// Get the total number of pages managed by the buffer
size_t Buffer::getTotalPages() const
{
    return totalPages;
}

bool Buffer::flushAllPages()
{
    bool success = true;
    for (auto &page : buffer)
    {
        success &= flushPage(page.first);
    }
    return success;
}

// Check if the buffer is full
bool Buffer::isFull() const
{
    return buffer.size() >= numOfPagesInBuffer;
}

// Flush a specific page to disk
bool Buffer::flushPage(size_t pageIndex)
{
    TRACE (true);
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        // Attempt to write the page to disk
        if(!it-> second -> getIsDirty() || disk -> writePagesToDisk(pageIndex * PAGE_SIZE, {it->second})){
            // Remove the page from the buffer
            buffer.erase(it);
            return true;
        }
    }
    return false; // Page not found in buffer or flush failed
}

bool Buffer::erasePage(size_t pageIndex){
    buffer.erase(buffer.find(pageIndex));
    return true;
}

// Replace a page in the buffer with a new page
bool Buffer::replacePage(size_t pageIndex, std::shared_ptr<Page> newPage){
    buffer.erase(buffer.find(pageIndex));
    buffer[pageIndex] = newPage;
    return true;
}