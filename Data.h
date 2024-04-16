#pragma once
#include "defs.h"
#include <vector>

class DataRecord {

public:
    DataRecord(const std::vector<byte>& data);
    std::vector<byte> serialize() const;
    std::vector<byte> key;
    std::vector<byte> fields;
};

class Page {
public:
    Page();
    Page(size_t recordSize);
    Page(size_t recordSize, const std::vector<byte>& data);
    
    std::vector<byte> serialize() const;
    bool addRecord(const DataRecord& record);
    const DataRecord* getRecord(size_t recordIndex) const;
    bool setPageIndex(size_t index);
    bool setRecord(size_t recordIndex, const DataRecord& record);
    bool getIsDirty() const;
    std::vector<DataRecord> getRecords() const;
    bool currentPageSize() const;
    size_t getPageIndex() const;
private:
    std::vector<DataRecord> records;
    const static size_t pageSize = PAGE_SIZE - sizeof(size_t); // store the number of records in the page
    size_t recordSize;
    size_t pageIndex;
    bool isDirty;
};