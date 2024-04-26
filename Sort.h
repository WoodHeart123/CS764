#include "Iterator.h"

struct runs{
	size_t startPageIndex;
	size_t endPageIndex;
}; 

class SortPlan : public Plan
{
	friend class SortIterator;
public:
	SortPlan (Plan * const input);
	~SortPlan ();
	Iterator * init () const;
private:
	Buffer* buffer;
	Plan * const _input;
	size_t numPages;
}; // class SortPlan

class SortIterator : public Iterator
{
public:
	SortIterator (SortPlan const * const plan);
	~SortIterator ();
	bool next ();
	bool sort(size_t startPageIndex, size_t endPageIndex);
private:
	SortPlan const * const _plan;
	std::vector<runs> runList;
	RowCount _produced;
}; // class SortIterator
