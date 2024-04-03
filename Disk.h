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
    bool readPageFromDisk(unsigned long long offset, Page* Pagebuffer);
    //if offset is larger than sdd file size, write to the hdd file
    bool writePageToDisk(unsigned long long offset, Page* Pagebuffer);
    // increment number of pages and write to 
    bool writeNewPageToDisk(unsigned long long offset, Page* Pagebuffer);

private:
    const std::string SSDName;
    const std::string HDDName;
    const static unsigned long long _SSDSize;
};