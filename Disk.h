#pragma once

#include "defs.h"
#include <string>
#include <memory>
#include "Data.h"

class Disk
{
public:
    // create ssd file and hdd file if not exist
    Disk();
    // create ssd file and hdd file if not exist
    Disk(std::string SSDName, std::string HDDName);
    // if offset is larger than sdd file size, read from the hdd file
    std::vector<std::shared_ptr<Page>> readPagesFromDisk(unsigned long long offset, size_t recordSize, size_t numPages);
    // if offset is larger than sdd file size, write to the hdd file
    bool writePagesToDisk(unsigned long long offset, std::vector<std::shared_ptr<Page>> pages);

private:
    const std::string SSDName;
    const std::string HDDName;
    const unsigned long long SSDSize = 10 * 1024ULL * 1024ULL * 1024ULL;
};