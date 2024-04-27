#include "Scan.h"
#include <vector>
#include <random>
#include <iostream>
#include <ctime>
#include <cctype>

ScanPlan::ScanPlan(RowCount const count, Buffer *buffer, size_t record_size) : count(count), record_size(record_size)
{
	this->buffer = buffer;
}

Iterator *ScanPlan::init() const
{
	return new ScanIterator(this);
}

ScanPlan::~ScanPlan()
{
}

ScanIterator::ScanIterator(ScanPlan const *const plan) : _plan(plan), currentRowCount(0)
{
	currentPage = _plan->buffer->createNewPage();
}

bool ScanIterator::next()
{
  TRACE(true);
	if (currentRowCount >= _plan->count)
	{
		if (currentPage && currentPage->getIsDirty())
		{
			_plan->buffer->flushPage(currentPage->getPageIndex());
		}
		return false;
	}
	DataRecord *newRecord = new DataRecord(generateAlphanumericVector(_plan->record_size));
	if (!currentPage->addRecord(*newRecord))
	{  
    if(_plan->buffer-> isFull()){
      _plan->buffer->flushAllPages();
    }
		// If page is full, flush it and get/create a new one
		currentPage = _plan->buffer->createNewPage();
		currentPage->addRecord(*newRecord);
	}
	delete newRecord;
	currentRowCount++;
	return true;
}

ScanIterator::~ScanIterator() {}

std::vector<byte> ScanIterator::generateAlphanumericVector(size_t length)
{
	std::vector<byte> result;
	result.reserve(length); // Reserve memory for efficiency

	// Random number generation setup
	std::random_device rd;						 // Seed
	std::mt19937 rng(rd());						 // Random number generator
	std::uniform_int_distribution<> dist(0, 61); // Range includes 26 lowercase + 26 uppercase + 10 digits

	for (size_t i = 0; i < length; ++i)
	{
		int randomIndex = dist(rng); // Get a random index
		char c;
		if (randomIndex < 26)
		{
			c = 'a' + randomIndex; // Lowercase letters
		}
		else if (randomIndex < 52)
		{
			c = 'A' + (randomIndex - 26); // Uppercase letters
		}
		else
		{
			c = '0' + (randomIndex - 52); // Numbers
		}
		result.push_back(static_cast<byte>(c));
	}
	return result;
}
