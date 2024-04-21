#include "Data.h"
#include <vector>
#include <stdexcept>

DataRecord::DataRecord(const std::vector<byte> &data)
{
    auto it = data.begin();
    key.insert(key.end(), it, it + 8);
    it += 8;
    fields.insert(fields.end(), it, data.end());
}

/*
serialize a row into a byte vector
*/
std::vector<byte> DataRecord::serialize() const
{
    std::vector<byte> data;
    data.reserve(key.size() + fields.size()); 
    data.insert(data.end(), key.begin(), key.end());
    data.insert(data.end(), fields.begin(), fields.end());
    return data;
}

Page::Page() : isDirty(false) {}

Page::Page(size_t recordSize) : recordSize(recordSize), isDirty(false) {}

Page::Page(size_t recordSize, const std::vector<byte> &data) : recordSize(recordSize), isDirty(false)
{
    auto it = data.begin();
    size_t numRecords = *reinterpret_cast<const size_t *>(&*it);
    it += sizeof(size_t);
    for (size_t i = 0; i < numRecords; i++)
    {
        size_t recordSize = *reinterpret_cast<const size_t *>(&*it);
        it += sizeof(size_t);
        std::vector<byte> recordData(it, it + recordSize);
        it += recordSize;
        DataRecord record(recordData);
        records.push_back(record);
    }
}

/**
 * serialize the page into a byte vector
*/
std::vector<byte> Page::serialize() const
{
    std::vector<byte> data;
    // store the number of records in the page
    size_t numRecords = records.size();
    data.insert(data.end(), reinterpret_cast<const byte *>(&numRecords), reinterpret_cast<const byte *>(&numRecords) + sizeof(numRecords));
    for (const auto &record : records)
    {
        auto recordData = record.serialize();
        data.insert(data.end(), recordData.begin(), recordData.end());
    }

    // padded with zeros
    size_t padding = PAGE_SIZE - data.size();
    data.resize(PAGE_SIZE, 0);

    return data;
}

bool Page::addRecord(const DataRecord &record)
{
    if (recordSize * (records.size() + 1ULL) > pageSize)
    {
       return false;
    }
    records.push_back(record);
    isDirty = true;
    return true;
}

const DataRecord *Page::getRecord(size_t recordIndex) const
{
    if (recordIndex >= records.size())
    {
        return (DataRecord *)nullptr;
    }
    return &records[recordIndex];
}

bool Page::setRecord(size_t recordIndex, const DataRecord &record)
{
    if (recordIndex >= records.size())
    {
        return false;
    }
    records[recordIndex] = record;
    isDirty = true;
    return true;
}

bool Page::setPageIndex(size_t index)
{
    if (index >= pageSize)
    {
        return false;
    }
    pageIndex = index;
    return true;
}

bool Page::currentPageSize() const
{
    return records.size() * recordSize;
}

bool Page::getIsDirty() const
{
    return isDirty;
}

std::vector<DataRecord> Page::getRecords() const
{
    return records;
}

size_t Page::getPageIndex() const
{
    return pageIndex;
}
