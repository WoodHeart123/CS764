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

/**
compare two records
*/
int DataRecord::cmp(const DataRecord &other) const
{
    size_t len1 = key.size();
    size_t len2 = other.key.size();
    size_t minLen = (len1 < len2) ? len1 : len2;

    for (size_t i = 0; i < minLen; ++i)
    {
        if (key[i] < other.key[i])
            return -1;
        if (key[i] > other.key[i])
            return 1;
    }

    if (len1 < len2)
        return -1;
    if (len1 > len2)
        return 1;
    return 0;
}

int DataRecord::keyCmp(const std::vector<byte> &key) const
{
    size_t len1 = this->key.size();
    size_t len2 = key.size();
    size_t minLen = (len1 < len2) ? len1 : len2;

    for (size_t i = 0; i < minLen; ++i)
    {
        if (this->key[i] < key[i])
            return -1;
        if (this->key[i] > key[i])
            return 1;
    }

    if (len1 < len2)
        return -1;
    if (len1 > len2)
        return 1;
    return 0;
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

size_t Page::size() const
{
    return records.size();
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
