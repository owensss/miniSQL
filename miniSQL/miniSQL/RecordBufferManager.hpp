#pragma once
#include "BufferManager.h"
#include "CatalogManager.h"

namespace record{
	//real struct mapping
	struct Tuple {
		void* header;	//info about attributes
		void* data;		//actual data
	};

	struct Block {
		void* header;
		Tuple* tuples;
	};
}

class RecordBufferManager
{
public:
	RecordBufferManager(void);
	~RecordBufferManager(void);

	char* getTuple(size_t row);
private:
	void write();	// convert data to binary and write to buffer
	void read();	// read from buffer and map to blocks
private:
	CatalogManager* catalogManager;	//info of that relation
	std::list<record::Block> blocks;	//blocks of that relation
	BufferManager* buffer;
};
