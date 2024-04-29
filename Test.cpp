#include "Iterator.h"
#include "Scan.h"
#include "Filter.h"
#include "Sort.h"
#include <fstream>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int opt;
	// default values
	int recordCount = 1000, recordSize = 100;
	std::string traceFileName = "trace.txt";
	while((opt = getopt(argc, argv, "c:s:o:")) != -1){
		switch(opt){
			case 'c':
				recordCount = std::stoi(optarg);
				break;
			case 's':
				recordSize = std::stoi(optarg);
				break;
			case 'o':
				traceFileName = optarg;
				break;
			default:
				printf("Wrong Parameter\n");
				return 1;
		}
	}

	Buffer *const buffer = new Buffer(recordSize);
	Plan *const plan = new SortPlan(new FilterPlan(new ScanPlan(recordCount, buffer, recordSize), greater, std::vector<byte>(8, 'Z')));
	Iterator *const it = plan->init();
	it->run();
	delete it;
	delete plan;

	// write all data to out.txt
	std::ofstream file("out", std::ios::binary | std::ios::out);
	if (!file)
	{
		std::cerr << "Failed to open out for writing.\n";
		return 1;
	}
	std::vector<byte> data;
	for (size_t i = 0; i < buffer->getTotalPages(); i++)
	{
		std::shared_ptr<Page> page = buffer->getExistingPage(i);
		for (auto record : page->getRecords())
		{	
			// write key and fields to file
			data = record.serialize();
			file.write(reinterpret_cast<const char *>(&data[0]), data.size());
			file.write("\n", 1);
		}
	}
	delete buffer;
	file.close();
	return 0;
} // main
