#include "RecordBufferManager.hpp"
#include "blockReadWrite.hpp"

const std::string RecordBufferManager::directoryName = "record";

RecordBufferManager::~RecordBufferManager(void)
{
}

// read from buffer and map to blocks
void RecordBufferManager::readBlock(const std::string& relationName, size_t block_num, size_t tupleLen){
	record::Block recordBlock;
	auto path = getFullName(relationName, block_num);
	memcpy(recordBlock.dataBlock.data, bufferManager->read(path), sizeof(BufferManager::BLOCK_SIZE));
	bufferManager->unlock(path);
	//get block data
	
	auto ptr = recordBlock.dataBlock.data;
	record::Head record_head;
	ptr = readBuffer(ptr, (record::DataPtr)&record_head, sizeof(record_head));//read head

	size_t tupleNum = record_head.totalTupleNum;

	for(size_t i = 0; i < tupleNum; i++){
		bool deleted;
		ptr = readBuffer(ptr, (record::DataPtr)&deleted, sizeof(bool));
		recordBlock.tuples.push_back(record::Tuple(deleted, ptr));
		ptr += tupleLen;
	}
	blocks.insert(std::pair<std::pair<std::string, size_t>, record::Block>
		(std::pair<std::string, size_t>(relationName, block_num), std::move(recordBlock)));
}

void RecordBufferManager::writeBlock(const std::string& relationName, size_t block_num) const{
	auto path = getFullName(relationName, block_num);
	auto &block = blocks.at(BlockIdPair(relationName, block_num));
	bufferManager->write(path, block.dataBlock.data);
	bufferManager->writeBack(path);
}
