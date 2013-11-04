#pragma once
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include <vector>

namespace record{
	//real struct mapping
	struct Head {
		size_t next;
		size_t tuple_num;
		bool terminate;
	};

	struct Tuple {
	//	void* header;	//info about the tuple, null or something
		void* data;		//actual data: int, chars, float
	};

	struct Block {
	public:
		typedef std::vector<Tuple> TupleSet;
	public:
		Head* header;
		TupleSet tuples;
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
