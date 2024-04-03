#pragma once

#include "Disk.h"
#include <fstream>
#include <iostream>



Disk::Disk() : SSDName("ssd.bin"), HDDName("hdd.bin") {
    std::ofstream ssdFile(SSDName, std::ios::binary | std::ios::app);
    std::ofstream hddFile(HDDName, std::ios::binary | std::ios::app);
}

Disk::Disk(std::string SSDName, std::string HDDName) : SSDName(SSDName), HDDName(HDDName) {
    std::ofstream ssdFile(SSDName, std::ios::binary | std::ios::app);
    std::ofstream hddFile(HDDName, std::ios::binary | std::ios::app);
}

bool Disk::readPageFromDisk(unsigned long long offset, Page* pageBuffer) {
    std::string fileName = offset < _SSDSize ? SSDName : HDDName;
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << fileName << " for reading.\n";
        return false;
    }

    // Adjust offset for HDD if necessary
    if (fileName == HDDName) {
        offset -= _SSDSize;
    }

    file.seekg(offset);
    if (!file.read(reinterpret_cast<char*>(pageBuffer), sizeof(Page))) {
        std::cerr << "Failed to read page from " << fileName << ".\n";
        return false;
    }

    return true;
}

bool Disk::writePageToDisk(unsigned long long offset, Page* pageBuffer) {
    std::string fileName = offset < _SSDSize ? SSDName : HDDName;
    std::ofstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Failed to open " << fileName << " for writing.\n";
        return false;
    }

    // Adjust offset for HDD if necessary
    if (fileName == HDDName) {
        offset -= _SSDSize;
    }

    file.seekp(offset);
    if (!file.write(reinterpret_cast<const char*>(pageBuffer->records), sizeof(Page))) {
        std::cerr << "Failed to write page to " << fileName << ".\n";
        return false;
    }

    return true;
}

bool Disk::writeNewPageToDisk(unsigned long long offset, Page* pageBuffer) {
    // Assuming this method is similar to writePageToDisk but possibly includes logic to increment page count or handle new pages differently
    return writePageToDisk(offset, pageBuffer);
}