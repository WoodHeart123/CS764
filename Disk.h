#pragma once

#include "defs.h"
#include <string>
#include <Data.h>
 

class Disk{
public:
    // create ssd file and hdd file if not exist
    Disk();
    // create ssd file and hdd file if not exist
    Disk(std::string SSDName, std::string HDDName);
    // if offset is larger than sdd file size, read from the hdd file
    std::vector<Page> readPagesFromDisk(unsigned long long offset, size_t recordSize, size_t numPages);
    //if offset is larger than sdd file size, write to the hdd file
    bool writePagesToDisk(unsigned long long offset, std::vector<Page> pages);

private:
    const std::string SSDName;
    const std::string HDDName;
    const static unsigned long long _SSDSize;
};