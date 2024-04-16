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

std::vector<Page> Disk::readPagesFromDisk(unsigned long long offset, size_t recordSize, size_t numPages) {
    std::string fileName = offset < _SSDSize ? SSDName : HDDName;
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << fileName << " for reading.\n";
    }

    // Adjust offset for HDD if necessary
    if (fileName == HDDName) {
        offset -= _SSDSize;
    }

    file.seekg(offset);
    std::vector<byte> data(PAGE_SIZE * numPages);
    // read page from disk
    if (!file.read(reinterpret_cast<char*>(&data[0]), PAGE_SIZE * numPages)) {
        std::cerr << "Failed to read page from " << fileName << ".\n";
    }

    std::vector<Page> pages;
    for (size_t i = 0; i < numPages; i++) {
        std::vector<byte> pageData(data.begin() + i * PAGE_SIZE, data.begin() + (i + 1) * PAGE_SIZE);
        Page* page = new Page(recordSize, pageData);
        pages.push_back(*page);
    }

    return pages;
}

bool Disk::writePagesToDisk(unsigned long long offset, std::vector<Page> pages) {
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

    std::vector<byte> data;
    for (const auto &page : pages) {
        auto pageData = page.serialize();
        data.insert(data.end(), pageData.begin(), pageData.end());
    }
    if (!file.write(reinterpret_cast<char*>(&data[0]), data.size())) {
        std::cerr << "Failed to write page to " << fileName << ".\n";
        return false;
    }

    return true;
}