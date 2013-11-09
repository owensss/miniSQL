#pragma once
#include "RecordRelationManager.hpp"
#include "blockReadWrite.hpp"
#include "RecordManager.hpp"
#include <algorithm>

namespace{
	//path: direcotory_name\relationName_blockNum
	inline const std::string getFullPath(const record::RelationManager& relationManager,  size_t block_num){
		return RecordManager::directoryName + "\\" + relationManager.getName() + "_"+std::to_string(block_num);
	}

	inline bool isFull(const record::Block& block, size_t tuple_len){
		size_t size = sizeof(record::Head) + block.header.totalTupleNum * tuple_len;
		return (size + tuple_len > BufferManager::BLOCK_SIZE);
	}
}

namespace record{
	const size_t RelationManager::GREATEST_BLOCKS_CAPACITY = 10;

	RelationManager::~RelationManager(){//write all info to files
		releaseBlocks(blocks.size());
	} 

	bool RelationManager::readBlock(size_t block_num){
		auto path = getFullPath(*this, block_num);
		record::Block block;
		try{
		memcpy(block.dataBlock.data, bufferManager->read(path), BufferManager::BLOCK_SIZE);
		bufferManager->unlock(path);//block data

		auto ptr = block.dataBlock.data;
		Head head;
		ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&head, sizeof(head));//get head
		
		auto tuple_num = head.totalTupleNum;
		for(size_t i = 0; i < tuple_num; i++){
			bool deleted;
			ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&deleted, sizeof(deleted));//get deleted info
			Tuple tuple(ptr, &block);
			ptr += tuple_len;
			TupleIter iter;
			if(deleted){
				deletedTuples.push_back(std::move(tuple));
				iter = deletedTuples.end()--;
			}
			else{
				nondeletedTuples.push_back(std::move(tuple));
				iter = nondeletedTuples.end()--;
			}

			block.tuples.push_back(iter);
		}
			blocks.push_back(std::move(block));
			return true;
		}//end of try
		catch(CannotOpenFile exn){//cannot access the file
			return false;
		}
	}

	void RelationManager::writeBlock(const Block& block){
		auto ptr = block.dataBlock.data;
		ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&block.header, sizeof(Head));
		//write head

		for(auto& tuple : block.tuples){
			ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&tuple->deleted, sizeof(bool));
			ptr += tuple_len;
		}//write tuple
	}

	void RelationManager::writeBlockToFile(const Block& block){
		auto path = getFullPath(*this, block.header.number);
		bufferManager->write(path, block.dataBlock.data);
		bufferManager->writeBack(path);
	}

	void RelationManager::deleteTuple(TupleIter tuple){
		tuple->deleted = true;
		auto & block = *tuple->block;
		block.header.totalTupleNum --;//decrement
		if(tuple->block->header.totalTupleNum == 0){//no tuples in the block
			remove(getFullPath(*this, block.header.number).c_str());//delete this block
			blocks.erase(findBlockByNumber(block.header.number));
			//remove that block from blocks
		}
		else
			nondeletedTuples.splice(tuple, deletedTuples);
	}

	void RelationManager::insertTuple(DataPtr data){
		while(blocks.empty() || (deletedTuples.empty() && isFull(blocks.back(), tuple_len))){
			releaseMemory();//in case of size too large
			addBlock(); //if no blocks or no empty room to insert keep adding blocks
		}
		if(!deletedTuples.empty()){//can replace those deleted tuple
			TupleIter tuple = deletedTuples.begin();//get first deletedTuple, reuse it
			*(bool*)(tuple->data) = false;//set not deleted
			memcpy(tuple->data+sizeof(bool), data, tuple_len);//copy data
			tuple->block->header.number ++;
			nondeletedTuples.splice(tuple, deletedTuples);
		}
		else{//have new not full block to write
			auto& block = blocks.back();
			auto ptr = block.tuples + tuple_len*block.header.number;
			*(bool*)ptr = false;	//not deleted
			ptr += sizeof(bool);
			memcpy(ptr, data, tuple_len);//copy data
			block.header.number ++;
			nondeletedTuples.push_back(Tuple(ptr, &blocks.back()));//add a new tuple
		}
	}

	void RelationManager::addBlock(void){
		size_t num;
		if(!blocks.empty()){
			auto& back = blocks.back();
			num = back.header.number + 1;
			back.header.terminate = false;
			//change in case it was terminate
		}
		else
			num = 0;//get num

		if(!readBlock(num)){//not success, no such file
			writeBlockToFile(Block(Head(num, 0, true), Block::TupleIterSet(), blockReadWrite::BlockData()));
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
		auto &block = std::find_if(blocks.begin(), blocks.end(), 
			[block_no](const Block& block){return block.header.number == block_no;});
		writeBlockToFile(*block);
	}

	void RelationManager::releaseBlock(const Block& block){
		writeBlockToFile(block);
		for(auto& tupleIter : block.tuples){
			if(tupleIter->)
		}
	}

	void RelationManager::releaseBlocks(size_t num){
		for(size_t i = 0; i < num; i++){
			auto& block = blocks.front();
			writeBlockToFile(block);
			blocks.pop_front();
		}//write and pop num elements
	}

	void RelationManager::deleteAll(void){
		deletedTuples.clear();
		nondeletedTuples.clear();

		while(!blocks.empty()){
			auto &block = blocks.front();
			remove(getFullPath(*this, block.header.number).c_str());
			blocks.pop_front();
		}
	}

	void RelationManager::insertBlock(const Block& block){
		auto& iter = blocks.begin();
		auto& end = blocks.end();

		for(	; iter != end && std::next(iter) != end; ){
			auto &next = std::next(iter);
			if(iter->header.number > next->header.number){
				iter = next;
				break;
			}
			else if(iter->header.number == next->header.number)
				return;	//if exists do nothing
			else
				iter = next;//continue
		}
		blocks.insert(iter, std::move(block));
	}

	std::pair<bool, RelationManager::TupleConstIter> RelationManager::getFirstTuple(void){
		if(blocks.at())
	}
}