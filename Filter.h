#include "Iterator.h"


enum Predicate{
	equal = 1
	greater
	less
	greaterEqual
	lessEqual
}

class FilterPlan : public Plan
{
	friend class FilterIterator;
public:
	FilterPlan (Plan * const input, Predicate predicate, std::vector<byte> value);
	~FilterPlan ();
	Iterator * init () const;
private:
	Plan * const _input;
	Predicate predicate;
	std::vector<byte> value;
}; // class FilterPlan

class FilterIterator : public Iterator
{
public:
	FilterIterator (FilterPlan const * const plan);
	~FilterIterator ();
	bool next ();
private:
	FilterPlan const * const _plan;
	Iterator * const _input;
	std::shared_ptr<Page> currentPage;
	RowCount _consumed, _produced;
}; // class FilterIterator
