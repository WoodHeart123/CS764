#include "Sort.h"
#include <algorithm>
#include <iostream>

SortPlan::SortPlan(Plan *const input, bool removeDuplicates) : _input(input), _removeDuplicates(removeDuplicates)
{
	this->buffer = _input->buffer;
	TRACE(true);
} // SortPlan::SortPlan

SortPlan::~SortPlan()
{
	TRACE(true);
	delete _input;
} // SortPlan::~SortPlan

Iterator *SortPlan::init() const
{
	TRACE(true);
	Iterator *const it = _input->init();
	it->run();
	delete it;
	return new SortIterator(this);
} // SortPlan::init

SortIterator::SortIterator(SortPlan const *const plan) : _plan(plan), _produced(0)
{
	TRACE(true);
	size_t totalPages = _plan->buffer->getTotalPages();
	_removeDuplicates = _plan->_removeDuplicates;
	// first sort the pages
	for (size_t i = 0; i < totalPages; i += _plan->buffer->numOfPagesInBuffer)
	{
		sort(i, std::min(i + _plan->buffer->numOfPagesInBuffer - 1, totalPages - 1));
		runList.push({i, std::min(i + _plan->buffer->numOfPagesInBuffer - 1, totalPages - 1), i});
	}
} // SortIterator::SortIterator

SortIterator::~SortIterator()
{
	TRACE(true);

	traceprintf("run %lu merge\n",
				(unsigned long)(_produced));
} // SortIterator::~SortIterator

bool SortIterator::sort(size_t startPageIndex, size_t endPageIndex)
{
	TRACE(true);
	std::cout << "STATE -> SORT_MINI_RUNS: Sort cache-size mini runs" << std::endl;
	_plan->buffer->clear();
	std::vector<std::shared_ptr<Page>> pages = _plan->buffer->getExistingPages(startPageIndex, endPageIndex);
	std::vector<DataRecord> records;
	for (auto page : pages)
	{
		for (auto record : page->getRecords())
		{
			records.push_back(record);
		}
		page->clear();
	}
	std::sort(records.begin(), records.end());
	std::shared_ptr<Page> page = pages.at(0);
	size_t i = 1;
	for (auto record : records)
	{
		if (!page->addRecord(record))
		{
			_plan->buffer->flushPage(page->getPageIndex());
			page = pages.at(i);
			page->addRecord(record);
			i++;
		}
	}
	if(startPageIndex <= HDD_PAGE_INDEX){
		float latency = static_cast<float>((std::min(static_cast<unsigned long long>(endPageIndex), HDD_PAGE_INDEX) - startPageIndex + 1) * PAGE_SIZE) / static_cast<float>(SSD_BANDWIDTH) + SSD_LATENCY;
		latency *= 1000000;
		std::cout << "STATE -> SPILL_RUNS_SSD: Spill sorted runs to the SSD device" << std::endl;
		std::cout << "ACCESS -> A write to SSD was made with size " << (std::min(static_cast<unsigned long long>(endPageIndex), HDD_PAGE_INDEX) - startPageIndex + 1) * PAGE_SIZE  << " bytes and latency " << latency << " us" << std::endl;
	}
	if(endPageIndex > HDD_PAGE_INDEX){
		float latency = static_cast<float>((static_cast<unsigned long long>(endPageIndex) - HDD_PAGE_INDEX + 1) * PAGE_SIZE) / static_cast<float>(HDD_BANDWIDTH) + HDD_LATENCY;
		latency *= 1000000;
		std::cout << "STATE -> SPILL_RUNS_HDD: Spill sorted runs to the HDD device" << std::endl;
		std::cout << "ACCESS -> A write to HDD was made with size " << (static_cast<unsigned long long>(endPageIndex) - HDD_PAGE_INDEX + 1) * PAGE_SIZE  << " bytes and latency " << latency << " us" << std::endl;

	}
	_plan->buffer->flushPage(page->getPageIndex());
	_plan->buffer->clear();
	return true;
} // SortIterator::sort

bool SortIterator::next()
{
	TRACE(true);
	if (runList.size() > 1)
	{
		std::vector<runs> mergeRunList;
		int k = runList.size();
		bool isSSD = runList.front().startPageIndex <= HDD_PAGE_INDEX;
		for (size_t i = 0; i < k && _plan->buffer->numOfPagesInBuffer; i++)
		{
			isSSD = isSSD && runList.front().startPageIndex <= HDD_PAGE_INDEX;
			mergeRunList.push_back(runList.front());
			runList.pop();
		}
		if(isSSD){
			std::cout << "STATE -> MERGE_RUNS_SSD: Merge sorted runs from the SSD device" << std::endl;
		}else{
			std::cout << "STATE -> MERGE_RUNS_HDD: Merge sorted runs from the HDD device" << std::endl;
		}
		LoserTree loserTree(mergeRunList, _plan->buffer);
		loserTree.initialize();
		std::shared_ptr<Page> newPage = _plan->buffer->createNewPage();
		size_t newStartPageIndex = newPage->getPageIndex(), newEndPageIndex;
		DataRecord record = loserTree.next();
		while (record.key.size() > 0)
		{
			if (!newPage->addRecord(record))
			{	
				if(newPage -> getPageIndex() > HDD_PAGE_INDEX){
					float latency = static_cast<float>(PAGE_SIZE) / static_cast<float>(HDD_BANDWIDTH) + HDD_LATENCY;
					latency *= 1000000;
					std::cout << "ACCESS -> A write to SSD was made with size " << PAGE_SIZE << " bytes and latency " << latency << " us" << std::endl;
				}else{
					float latency = static_cast<float>(PAGE_SIZE) / static_cast<float>(SSD_BANDWIDTH) + SSD_LATENCY;
					latency *= 1000000;
					std::cout << "ACCESS -> A write to SSD was made with size " << PAGE_SIZE << " bytes and latency " << latency << " us" << std::endl;
				}
				_plan->buffer->flushPage(newPage->getPageIndex());
				newPage = _plan->buffer->createNewPage();
				newPage->addRecord(record);
			}
			record = loserTree.next();
		} 
		newEndPageIndex = newPage->getPageIndex();
		// add the new run to the runList
		runList.push({newStartPageIndex, newEndPageIndex, newStartPageIndex});
		_produced++;
		return true;
	}
	else
	{	
		// flush the last run and also verifying the sort orders
		size_t newPageIndex = 0;
		for(size_t i = runList.front().startPageIndex; i <= runList.front().endPageIndex; i++){
			std::shared_ptr<Page> page = _plan->buffer->getExistingPage(i);
			if (page->getRecords().size() > 1)
			{
				for (size_t j = 1; j < page->getRecords().size(); j++)
				{
					if (page->getRecords().at(j).key < page->getRecords().at(j - 1).key)
					{
						std::cerr << "Error: Sort order violated" << std::endl;
					}
				}
			}
			_plan->buffer->replacePage(newPageIndex, page);
			_plan->buffer->flushPage(newPageIndex);
			newPageIndex++;
		}
		_plan->buffer->setTotalPages(newPageIndex);
		return false;
	}
} // SortIterator::next
