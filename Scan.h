#include "Iterator.h"


class ScanPlan : public Plan
{
	friend class ScanIterator;
public:
	ScanPlan (RowCount const count, Buffer* buffer, size_t record_size);
	~ScanPlan ();
	Iterator * init () const;
private:
	RowCount const count;
	size_t record_size;
}; // class ScanPlan

class ScanIterator : public Iterator
{
public:
	ScanIterator (ScanPlan const * const plan);
	~ScanIterator ();
	bool next ();
	std::vector<byte> generateAlphanumericVector(size_t length);
private:
	ScanPlan const * const _plan;
	RowCount currentRowCount;
	std::shared_ptr<Page> currentPage;
}; // class ScanIterator
