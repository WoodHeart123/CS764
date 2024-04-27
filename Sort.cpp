#include "Sort.h"
#include<algorithm>

SortPlan::SortPlan (Plan * const input) : _input (input)
{
  this -> buffer = _input -> buffer;
	TRACE (true);
} // SortPlan::SortPlan

SortPlan::~SortPlan ()
{
	TRACE (true);
	delete _input;
} // SortPlan::~SortPlan

Iterator * SortPlan::init () const
{
	TRACE (true);
	Iterator *const it = _input->init();
	it->run();
	delete it;
	return new SortIterator (this);
} // SortPlan::init

SortIterator::SortIterator (SortPlan const * const plan) :
	_plan (plan),  _produced (0)
{
	TRACE (true);
} // SortIterator::SortIterator

SortIterator::~SortIterator ()
{
	TRACE (true);

	traceprintf ("produced %lu rows\n",
			(unsigned long) (_produced));
} // SortIterator::~SortIterator

bool SortIterator::sort (size_t startPageIndex, size_t endPageIndex)
{
	TRACE (true);
	_plan -> buffer -> clear();
	std::vector<std::shared_ptr<Page>> pages = _plan -> buffer -> getExistingPages(startPageIndex, endPageIndex);
	std::vector<DataRecord> records;
	for (auto page : pages)
	{
		for (auto record : page -> getRecords())
		{
			records.push_back(record);
		}
		page -> getRecords().clear();
	}
	std::sort(records.begin(), records.end());
	std::shared_ptr<Page> page = pages.at(0);
	size_t i = 1;
	for (auto record : records)
	{
		if (!page -> addRecord(record))
		{
			page = pages.at(i++);
			page -> addRecord(record);
		}
	}
	_plan -> buffer -> flushAllPages();
	_plan -> buffer -> clear();
	return true;
} // SortIterator::sort

bool SortIterator::next ()
{
	TRACE (true);
  size_t totalPages = _plan -> buffer -> getTotalPages();
	// first sort the pages
	for (size_t i = 0; i < totalPages; i+= _plan -> buffer -> numOfPagesInBuffer)
	{
		sort(i, std::min(i + _plan -> buffer -> numOfPagesInBuffer - 1, totalPages - 1));
		runList.push_back({i, i + _plan -> buffer -> numOfPagesInBuffer - 1});
	}


	if(runList.size() == 1)
	{
		// only one run
		std::shared_ptr<Page> page = _plan -> buffer -> getExistingPage(runList.at(0).startPageIndex);
		for (auto record : page -> getRecords())
		{
			_produced++;
		}
		return false;
	}else{
		printf("Multiple runs %d\n", runList.size());
	}
	return false;
} // SortIterator::next
