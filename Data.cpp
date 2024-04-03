#include "Data.h"
#include <vector>
#include <stdexcept> 


Page::Page() : isDirty(false) {}

void Page::addRecord(const DataRecord& record) {
    records.push_back(record);
    isDirty = true;
}

const DataRecord* Page::getRecord(size_t recordIndex) const {
    if (recordIndex >= records.size()) {
        return (DataRecord*) nullptr;
    }
    return &records[recordIndex];
}

bool Page::setRecord(size_t recordIndex, const DataRecord& record) {
    if (recordIndex >= records.size()) {
        return false;
    }
    records[recordIndex] = record;
    isDirty = true;
    return true;
}

bool Page::getIsDirty() const {
    return isDirty;
}

std::vector<DataRecord> Page::getRecords() const {
    return records;
}
