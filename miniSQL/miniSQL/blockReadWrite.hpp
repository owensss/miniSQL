#pragma once

/** used for block operation
  */
#include "BufferManager.hpp"
#include <cstdint>
#include <cstring>
#include <string>

namespace blockReadWrite{
/** used to manage a block's memory
  */
struct BlockData{
public:
	BlockData(void){
		data = new byte[BufferManager::BLOCK_SIZE];
	}
	~BlockData(){
		delete [] data;
	}
public:
	BufferManager::DataPtr data;
};
}
/** memcpy and add offset to the pointer returned
  */
inline const BufferManager::DataPtr writeBuffer(BufferManager::DataPtr dest, const BufferManager::DataPtr source, size_t size){
	memcpy(dest, source, size);
	return dest + size;
}

/** memcpy the chars of string type to some place
  */
inline const BufferManager::DataPtr writeStringToBuffer(BufferManager::DataPtr dest, const std::string& string){
	const char *str = string.c_str();
	return writeBuffer(dest, (BufferManager::DataPtr)str, strlen(str)+1);
}

/** read from dest to source
  */
inline const BufferManager::DataPtr readBuffer(BufferManager::DataPtr dest, const BufferManager::DataPtr source, size_t size){
	memcpy(source, dest, size);
	return dest + size;
}

/** will automatically increment the source
  */
inline const std::string readStringFromBuffer(BufferManager::DataPtr& source){
	const std::string str((char *)source);
	source += strlen((char*)source) + 1;
	return str;
} 