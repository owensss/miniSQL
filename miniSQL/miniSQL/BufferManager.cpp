#include "BufferManager.hpp"
#include <fstream>
BufferManager::BufferManager(void)
{
	contents = new byte[BLOCK_SIZE*BLOCK_NUM];
}


BufferManager::~BufferManager(void)
{
	delete []contents;
}

BufferManager::block_iter BufferManager::getBlock(const std::string& filePath){
	auto it = list.begin();
	auto end = list.end();

	for( ; it != end && it->filePath != filePath; it++) ;
	return it;
}

BufferManager::block_const_iter BufferManager::getBlock(const std::string& filePath) const{
	auto it = list.begin();
	auto end = list.end();

	for( ; it != end && it->filePath != filePath; it++) ;
	return it;
}

bool BufferManager::lock(const std::string& filePath){
	auto block = getBlock(filePath);
	if(block == list.end())
		return false;
	else{
		block->pin = true;
		return true;
	}
}

bool BufferManager::unlock(const std::string& filePath){
	auto block = getBlock(filePath);
	if(block == list.end())
		return false;
	else{
		block->pin = false;
		return true;
	}
}

void* BufferManager::read(const std::string& filePath){
	auto block = getBlock(filePath);
	if(block != list.end()){//inside buffer
		block->pin = true;
		auto address = getAddr(block);
		pullForward(block);
		return address;
	}
	else{//not inside buffer
		auto block = addBlock(filePath);
		block->pin = true;
		auto address = getAddr(block);

		//read in data
		std::ifstream is(filePath, std::ifstream::binary);
		if(is.good()){
			is.read((char *)address, BLOCK_SIZE);
			is.close();
		}
		else
			throw CannotOpenFile(filePath.c_str());

		return address;
	}
}

BufferManager::block_const_iter BufferManager::LRU_substitution(void) const{
	auto rit = list.rbegin();
	auto end = list.rend();
	
	for(; rit != end && rit->pin == true; ++rit)	; //find the last unpined block
	if(rit == end)
		throw NoSubstitution();

	return (++rit).base();
}

BufferManager::block_iter BufferManager::LRU_substitution(void){
	auto rit = list.rbegin();
	auto end = list.rend();
	
	for(; rit != end && rit->pin == false; rit++)	; //find the last unpined block
	if(rit == end)
		throw NoSubstitution();

	return (++rit).base();
}
BufferManager::block_iter BufferManager::addBlock(const std::string& filePath){
	if(list.size() < BLOCK_NUM){//buffer not filled
		list.push_front(buffer::Block(filePath, list.size(), false, false));
		//add a new block to the buffer
		return list.begin();
	}
	else{//buffer filled
		auto newBlock = LRU_substitution();
		newBlock->filePath = filePath;
		newBlock->dirt = false;
		newBlock->pin = false;
		//overwrite data of victim
		return newBlock;
	}
}

void BufferManager::write(const std::string& filePath, void *source){
	block_iter block = getBlock(filePath);
	void* dest = nullptr;
	if(block == list.end()){//not in the list
		block = addBlock(filePath);
		dest = getAddr(block);
	}
	else{
		dest = getAddr(block);
	}
	memcpy(dest, source, BLOCK_SIZE);
	block->dirt = true;
	block->pin = true;
	pullForward(block);
}

void BufferManager::writeBack(const std::string& filePath){
	auto block = getBlock(filePath);
	if(block == list.end())//not in buffer
		return;
	else{
		void* addr = getAddr(block);
		std::ofstream of(filePath, std::ifstream::binary);
		if(of.good()){
			of.write((char *)addr, BLOCK_SIZE);
			of.close();
			block->dirt = false;
		}
		else
			throw CannotOpenFile(filePath.c_str());
	}
}

BufferManager::block_const_iter BufferManager::findBlockByAddr(void* addr) const{
	size_t index = ((byte *)addr - (byte *)contents) / BLOCK_SIZE;
	auto end = list.end();
	auto it = list.begin();
	for( ; it != end && it->index != index; it++)	;
	return it;
}

BufferManager::block_iter BufferManager::findBlockByAddr(void* addr){
	size_t index = ((byte *)addr - (byte *)contents) / BLOCK_SIZE;
	auto end = list.end();
	auto it = list.begin();
	for( ; it != end && it->index != index; it++)	;
	return it;
}

void BufferManager::writeMemory(void *dest, void *source, size_t size) {
	memcpy(dest, source, size);
	auto block = findBlockByAddr(dest);
	block->dirt = true;
	pullForward(block);	 
}