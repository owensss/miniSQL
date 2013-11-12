#pragma once
#include "RecordRelationManager.hpp"
#include "blockReadWrite.hpp"
#include "RecordManager.hpp"
#include "fileOperation.hpp"
#include <algorithm>

namespace{
	//path: direcotory_name\relationName_blockNum
	inline const std::string getFullPath(const record::RelationManager& relationManager,  size_t block_num){
		return RecordManager::directoryName + "\\" + relationManager.getName() + "_"+std::to_string(block_num);
	}

	//get the offset of freeSpace
	inline record::Tuple::TupleOffset getFreeSpaceOff(const record::Block& block, size_t tuple_len){
		return sizeof(record::Head) + block.header.totalTupleNum * (tuple_len + sizeof(record::Tuple::TupleOffset));
	}

	inline bool isFull(const record::Block& block, size_t tuple_len){
		return getFreeSpaceOff(block, tuple_len) >= BufferManager::BLOCK_SIZE;
	}
}

namespace record{
	const size_t RelationManager::GREATEST_BLOCKS_CAPACITY = 10; //buffer 10 blocks for one relation

	RelationManager::~RelationManager(){//write all info to files
		releaseBlocks(blocks.size());
	} 

	//create a dataBlock and write and write that block to disk
	void RelationManager::writeBlock(const Block& block){
		blockReadWrite::BlockData dataBlock;
		DataPtr ptr = dataBlock.data;

		ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&block.header, sizeof(Head));
		//write head

		for(auto& tuple : block.tuples){
			ptr = blockReadWrite::writeBuffer(ptr, tuple.data, sizeof(tuple_len)); //write tuple data
			ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&tuple.nextDeleted, sizeof(Tuple::TupleOffset));
		}//write tuple

		auto &path = getFullPath(*this, block.header.number);
		bufferManager->write(path, ptr);
		bufferManager->writeBack(path); //save
	}

	/* read existed block with its number, if cannot access return false
	 * user should ensure that block not in blockSet
     */
	bool RelationManager::readBlock(size_t block_num){
		auto path = getFullPath(*this, block_num);
		try{
			DataPtr block_addr = bufferManager->read(path);
			record::Block recordBlock;
			Head &head = recordBlock.header;
			auto ptr = blockReadWrite::readBuffer(block_addr, (DataPtr)&head, sizeof(head)); //get head
			
			auto tuple_num = head.totalTupleNum;
			auto next_deleted = head.firstDeleted;
			for(size_t i = 0; i < tuple_num; i++){
				if(ptr == block_addr + next_deleted){ //get to the deleted tuple
					ptr += tuple_len; //skip tuple contents
					ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&next_deleted, sizeof(Tuple::TupleOffset));
					//get addr of next delete
					continue; //skip
				}
				else{ //nondeleted tuple
					Tuple tuple(ptr, Tuple::NULL_OFFSET, &recordBlock);
					ptr += tuple_len + sizeof(Tuple::TupleOffset);
					recordBlock.tuples.push_back(std::move(tuple));
				}
			}
			blocks.insert(BlockPair(head.number, std::move(recordBlock)));
			return true;
		}//end of try
		catch(CannotOpenFile exn){//cannot access the file
			return false;
		}
	}

	void RelationManager::deleteTuple(TupleIter tuple){
		auto & block = *tuple->block;
		auto &head = block.header;
		tuple->nextDeleted = head.firstDeleted;
		head.firstDeleted = tuple->data - bufferManager->read(getFullPath(*this, head.number));
		//tuple.nextDeleted

		if(tuple->block->header.totalTupleNum == 0){//no tuples in the block
			remove(getFullPath(*this, block.header.number).c_str());//delete the file
			blocks.erase(block.header.number);
			//remove that block from blocks
		}
	}

	/* insert tuple into one block
	 * used by insertTuple, user should ensure that the block has free space
	 */
	bool RelationManager::doInsertTuple(Block& block, DataPtr data){
		if(block.header.firstDeleted == Tuple::NULL_OFFSET && isFull(block, tuple_len))
			return false;

		auto addr = bufferManager->read(getFullPath(*this, block.header.number));
		auto &head = block.header;
		if(block.header.firstDeleted != Tuple::NULL_OFFSET){
			//use the deleted not inc totalTupleNum
			addr += block.header.firstDeleted;
			blockReadWrite::readBuffer(addr+tuple_len, (DataPtr)&head.firstDeleted, sizeof(Tuple::TupleOffset));
			//get next deleted addr
		}
		else { //head.firstDeleted == NULL_OFFSET
			head.totalTupleNum ++;
			addr +=getFreeSpaceOff(block, tuple_len);
		}
		bufferManager->writeMemory(addr, data, tuple_len);	//write data to mem
		block.tuples.push_back(Tuple(addr, &block)); //add to tuples
		return true;
	}

	bool RelationManager::insertTuple(DataPtr data){
		for(auto& blockPair : blocks){//for all the blocks inside memory
			if(doInsertTuple(blockPair.second, data)) //if successfully insert
				break;
		}

		//no free memory in blocks, find backward for free space if no then create one
		do{
			appendBlock();
			releaseMemory();
		}while(!doInsertTuple(blocks.rbegin()->second, data));
		//while no space to insert keep appending
		return true; //TODO: may change the return later
	}

	// add block to tail, if not exists then create one
	void RelationManager::appendBlock(void){
		size_t num;
		if(!blocks.empty()){
			auto& largest_block_head = blocks.rbegin()->second.header;
			num = largest_block_head.number + 1;
			largest_block_head.terminate = false;
			//change in case it was terminate
		}
		else
			num = 0;//get num

		if(!readBlock(num)){//if not success (no such file) => create one
			writeBlock(Block(Head(num, 0, true, Tuple::NULL_OFFSET)));
			//just to create the file
			readBlock(num);//get block
		}
	}

	void RelationManager::releaseMemory(void){
		if(blocks.size() < GREATEST_BLOCKS_CAPACITY)
			return;
		else{
			releaseBlocks(blocks.size()/2);
			releaseMemory();//release until under the line
		}
	}

	void RelationManager::releaseBlock(size_t block_no){
		auto &block = blocks.at(block_no);
		writeBlock(block);
		bufferManager->unlock(getFullPath(*this, block_no));
	}

	/* release num blocks
	 * user should ensure that num not exceeds the actual size
	 */
	void RelationManager::releaseBlocks(size_t num){
		auto tail_num = blocks.rbegin()->second.header.number;
		size_t cnt = 0;
		for(size_t i = 0; cnt < num; i++){
			auto &block_ptr = blocks.find(i + tail_num);
			if(block_ptr != blocks.end()){
				releaseBlock(i + tail_num);
				cnt++;
			}
		}
	}

	void RelationManager::deleteAll(void){ //delete the whole relation
		auto fileNames = getFileNamesWithPrefix((RecordManager::directoryName + "\\" + name + "_").c_str());
		for(auto& fileName: fileNames)
			remove(fileName.c_str());
	}

	std::pair<bool, RelationManager::TupleIter> RelationManager::getFirstTuple(size_t block_num){
		if(!readBlock(block_num) || getBlock(block_num).tuples.empty()) //no such file or no tuples
			return std::pair<bool, RelationManager::TupleIter>(false, TupleSet().end());
		else
			return std::pair<bool, RelationManager::TupleIter>(false, getBlock(block_num).tuples.begin());
	}

	std::pair<bool, RelationManager::TupleIter> RelationManager::getNextTuple(TupleIter tuple){
		if(tuple != tuple->block->tuples.end()) //not reach the end of the block yet
			return std::pair<bool, RelationManager::TupleIter>(true, ++tuple);
		else{//get next block
			return getFirstTuple(tuple->block->header.number);
		}
	}
}