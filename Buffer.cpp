#include "Buffer.h"
#include <vector>
#include <string>
#include <iostream>

Buffer::Buffer(size_t size) : totalPages(0), recordSize(size)
{
    buffer.reserve(numOfPagesInBuffer);
    disk = new Disk();
}

// Create a new page and add it to the buffer
Page *Buffer::createNewPage()
{
    if (buffer.size() >= numOfPagesInBuffer)
    {
        std::cerr << "Buffer is full, consider flushing pages to disk." << std::endl;
        return (Page*) nullptr; // Buffer is full
    }
    Page newPage = Page(recordSize);
    size_t newPageIndex = totalPages;
    buffer[newPageIndex] = newPage;
    totalPages++;
    return &buffer[newPageIndex];
}

// Get an existing page from buffer or from disk if not in buffer
Page *Buffer::getExistingPage(size_t pageIndex)
{
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        return &buffer.at(pageIndex); // Page found in buffer
    }
    else
    {
        // Load page from disk if not found in buffer
        std::vector<Page> pages = disk -> readPagesFromDisk(pageIndex * PAGE_SIZE, sizeof(DataRecord), 1);
        if (!pages.empty())
        {
            // Assume only one page was read
            buffer[pageIndex] = pages[0];
            return &buffer.at(pageIndex);
        }
    }
    return (Page*) nullptr; // Page not found in buffer and could not be loaded from disk
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
    auto it = buffer.find(pageIndex);
    if (it != buffer.end())
    {
        // Attempt to write the page to disk
        if(!it-> second.getIsDirty() || disk.writePagesToDisk(pageIndex * PAGE_SIZE, {it->second})){
            // Remove the page from the buffer
            buffer.erase(it);
            return true;
        }
    }
    return false; // Page not found in buffer or flush failed
}