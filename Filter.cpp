#include "Filter.h"

FilterPlan::FilterPlan(Plan *const input, Predicate predicate, std::vector<byte> value) : _input(input), predicate(predicate), value(value)
{
	buffer = input->buffer;
} // FilterPlan::FilterPlan

FilterPlan::~FilterPlan()
{
	TRACE(true);
	delete _input;
} // FilterPlan::~FilterPlan

Iterator *FilterPlan::init() const
{
	TRACE(true);
	Iterator *const it = _input->init();
	it->run();
	delete it;
	return new FilterIterator(this);
} // FilterPlan::init

FilterIterator::FilterIterator(FilterPlan const *const plan)
	: _plan(plan)
{
	_currentIndex = 0;
	totalPages = _plan->buffer->getTotalPages();
	currentPageIndex = 0;
	newPageIndex = 0;
	_consumed = 0;
	_produced = 0;
	currentPage = _plan->buffer->getExistingPage(currentPageIndex++);
	newPage = _plan->buffer->createNewPage();
}

FilterIterator::~FilterIterator()
{
	TRACE(true);

	traceprintf("produced %lu of %lu rows\n",
				(unsigned long)(_produced),
				(unsigned long)(_consumed));
} // FilterIterator::~FilterIterator

bool FilterIterator::next()
{
	TRACE(true);

	if (_currentIndex >= currentPage->size())
	{
		if (currentPageIndex >= totalPages)
		{
			if (newPage && newPage->getIsDirty())
			{
				_plan->buffer->replacePage(newPageIndex, newPage);
				_plan->buffer->flushPage(newPageIndex);
				newPageIndex++;
			}
			_plan-> buffer -> setTotalPages(newPageIndex);
			return false;
		}

		currentPage = _plan->buffer->getExistingPage(currentPageIndex++);
		_currentIndex = 0;
	}

	const DataRecord *record = currentPage->getRecord(_currentIndex);
	if (match(*record, _plan->value))
	{
		_produced++;
		if (!newPage->addRecord(*record))
		{
			// If page is full, flush it and get/create a new one
			_plan->buffer->replacePage(newPageIndex, newPage);
			_plan->buffer->flushPage(newPageIndex);
			newPageIndex++;
			newPage = _plan->buffer->createNewPage();
			newPage->addRecord(*record);
		}
	}
	else
	{
		_consumed++;
	}
	_currentIndex++;
	return true;
} // FilterIterator::next

bool FilterIterator::match(const DataRecord &record, const std::vector<byte> &value)
{
	switch (_plan->predicate)
	{
	case equal:
		return record.keyCmp(value) == 0;
	case notEqual:
		return record.keyCmp(value) != 0;
	case greater:
		return record.keyCmp(value) > 0;
	case less:
		return record.keyCmp(value) < 0;
	case greaterEqual:
		return record.keyCmp(value) >= 0;
	case lessEqual:
		return record.keyCmp(value) <= 0;
	default:
		return false;
	}
}
