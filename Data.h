#pragma once
#include "defs.h"
#include <vector>

struct DataRecord {
    std::vector<byte> key;
    std::vector<byte> fields;
};

class Page {
public:
    Page();
    Page(size_t recordSize);
    void addRecord(const DataRecord& record);
    const DataRecord* getRecord(size_t recordIndex) const;
    bool setRecord(size_t recordIndex, const DataRecord& record);
    bool getIsDirty() const;
    std::vector<DataRecord> getRecords() const;

private:
    std::vector<DataRecord> records;
    const static size_t pageSize = PAGE_SIZE;
    size_t recordSize;
    size_t pageIndex;
    bool isDirty;
};