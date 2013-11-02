#include "stdafx.h"
#include "../MiniSQL/BufferManager.h"
#include <iostream>

#define WRITE
int main(void){
	BufferManager bufferManager;
#ifdef WRITE
	void *ptr = new char[4*1024];
	strcpy((char*)ptr, "Hello, DataBase");
	bufferManager.write("block1",ptr);
	bufferManager.writeBack("block1");
	std::cout << "write block" << std::endl;
#endif
#ifdef READ
	void *ptr = bufferManager.read("block1");
	std::cout << (char *)ptr << std::endl;
	getchar();
#endif
	
	return 0;
}