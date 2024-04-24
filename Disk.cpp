#pragma once

#include "Disk.h"
#include <fstream>
#include <iostream>

Disk::Disk() : SSDName("ssd.bin"), HDDName("hdd.bin")
{
    std::ofstream ssdFile(SSDName, std::ios::binary | std::ios::app);
    std::ofstream hddFile(HDDName, std::ios::binary | std::ios::app);
}

Disk::Disk(std::string SSDName, std::string HDDName) : SSDName(SSDName), HDDName(HDDName)
{
    std::ofstream ssdFile(SSDName, std::ios::binary | std::ios::app);
    std::ofstream hddFile(HDDName, std::ios::binary | std::ios::app);
}

std::vector<std::shared_ptr<Page>> Disk::readPagesFromDisk(unsigned long long offset, size_t recordSize, size_t numPages)
{
    TRACE(true);
    std::string fileName = offset < SSDSize ? SSDName : HDDName;
    std::ifstream file(fileName, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open " << fileName << " for reading.\n";
    }

    // Adjust offset for HDD if necessary
    if (fileName == HDDName)
    {
        offset -= SSDSize;
    }

    file.seekg(offset);
    std::vector<byte> data(PAGE_SIZE * numPages);
    // read page from disk
    if (!file.read(reinterpret_cast<char *>(&data[0]), PAGE_SIZE * numPages))
    {
        std::cerr << "Failed to read page from " << fileName << ".\n";
    }

    std::vector<std::shared_ptr<Page>> pages;
    for (size_t i = 0; i < numPages; i++)
    {
        std::vector<byte> pageData(data.begin() + i * PAGE_SIZE, data.begin() + (i + 1) * PAGE_SIZE);
        pages.push_back(std::shared_ptr<Page>(new Page(recordSize, pageData)));
    }
    return pages;
}

bool Disk::writePagesToDisk(unsigned long long offset, std::vector<std::shared_ptr<Page>> pages)
{
    TRACE(true);
    std::string fileName = offset < SSDSize ? SSDName : HDDName;
    std::ofstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!file)
    {
        std::cerr << "Failed to open " << fileName << " for writing.\n";
        return false;
    }

    // Adjust offset for HDD if necessary
    if (fileName == HDDName)
    {
        offset -= SSDSize;
    }

    file.seekp(offset);

    std::vector<byte> data;
    for (const auto &page : pages)
    {
        auto pageData = page->serialize();
        data.insert(data.end(), pageData.begin(), pageData.end());
    }
    if (!file.write(reinterpret_cast<char *>(&data[0]), data.size()))
    {
        std::cerr << "Failed to write page to " << fileName << ".\n";
        return false;
    }

    return true;
}