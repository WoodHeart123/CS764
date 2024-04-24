#pragma once

#include "defs.h"
#include "Buffer.h"

typedef uint64_t RowCount;

class Plan
{
	friend class Iterator;
public:
	Plan ();
	virtual ~Plan ();
	virtual class Iterator * init () const = 0;
	Buffer* buffer;
}; // class Plan

class Iterator
{
public:
	Iterator ();
	virtual ~Iterator ();
	void run ();
	virtual bool next () = 0;
	RowCount _count;
	RowCount _currentIndex;
}; // class Iterator
