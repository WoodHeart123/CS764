#include "Iterator.h"
#include "LoserTree.h"
#include <queue>
#include <vector>

class SortPlan : public Plan
{
	friend class SortIterator;
public:
	SortPlan (Plan * const input, bool removeDuplicates = false);
	~SortPlan ();
	Iterator * init () const;
private:
	Buffer* buffer;
	Plan * const _input;
	size_t numPages;
	bool _removeDuplicates;
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
	std::queue<runs> runList;
	bool _removeDuplicates;
	RowCount _produced;
}; // class SortIterator
