#include "Iterator.h"
#include "Scan.h"
#include "Filter.h"
#include "Sort.h"

int main (int argc, char * argv [])
{
	TRACE (true);

	Buffer * const buffer = new Buffer ();
	Plan * const plan = new ScanPlan (7, buffer, 120);
	// new SortPlan ( new FilterPlan ( new ScanPlan (7) ) );

	Iterator * const it = plan->init ();
	it->run ();
	delete it;

	delete plan;

	return 0;
} // main