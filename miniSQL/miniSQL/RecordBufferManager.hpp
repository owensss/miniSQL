#pragma once
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include "blockReadWrite.hpp"
#include <vector>
class RecordBufferManager;

/** translate block data between head and tuples
  * just with one record at one time
  * user shall release the record after done its operation 
  * (delete, selection, insert)
  */

class RecordBufferManager
{
public:
	RecordBufferManager(CatalogManager* catalogManager, BufferManager* bufferManager)
		:catalogManager(catalogManager), bufferManager(bufferManager){}
	~RecordBufferManager(void);

	/** will add the relation to the manager if not exists
	  * need client release the relation later
	  */
	TupleIter getNextTupleIter(TupleIter iter);
	TupleIter getFirstTupleIter(const std::string& realtion_name);
	void ReleaseRelation(const std::string& realtion_name);
	
	void deleteTuple(TupleIter iter){ //lazy deletion
		iter->deleted = true;
	}

	void insertTuple(const record::Tuple& tuple);	//insert tuple to somewhere in the record
public:
	tupleList::iterator TupleIter;
private:
	typedef std::map<std::pair<std::string, size_t>, record::Block> BlockSet;//map((relationName, no), block)
	typedef std::pair<std::string, size_t> BlockIdPair;
	typedef std::pair<std::pair<std::string, size_t>, record::Block> BlockPair;
	typedef std::list<record::Tuple> tupleList;
	const static std::string directoryName;
private:
	/** record named like relationName_i (i start from 0)
	  * this function gives the full path for the block
	  */
	std::string getFullName(const std::string& relation_name, size_t block_num) const{
		return directoryName + "\\" + relation_name+"_"+std::to_string(block_num);
	}
	// convert data to binary and write to buffer
	void writeBlock(const std::string& relationName, size_t block_num) const;
	void readBlock(const std::string& relationName, size_t block_num, size_t tupleLen);// read from buffer and map to blocks
private:
	BufferManager* bufferManager;
	CatalogManager* catalogManager;
	BlockSet blocks;	//blocks of that relation
	tupleList deletedTuple;
	tupleList nonDeletedTuple;
};