#include "Iterator.h"
#include "Scan.h"
#include "Filter.h"
#include "Sort.h"

int main (int argc, char * argv [])
{
	TRACE (true);

	Buffer * const buffer = new Buffer(120);
	Plan * const plan = new ScanPlan (700, buffer, 1024);
	// new SortPlan ( new FilterPlan ( new ScanPlan (7) ) );

	Iterator * const it = plan->init ();
	it->run ();
	delete it;
  
	delete plan;
 
  delete buffer;

	return 0;
} // main
