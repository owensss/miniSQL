#pragma once
#include <utility>
#include "BufferManager.hpp"

namespace record{
	typedef BufferManager::DataPtr DataPtr;
	struct Block;
	class RelationManager;
	
	/** write into the file, head of block
	  */
	class Tuple{
		friend class RelationManager; //can be accessed by relationManager
	public:
		typedef size_t TupleOffset;
		const static size_t NULL_OFFSET;// 0xFFFF
	public:
		Tuple(BufferManager::DataPtr data, Block* block) //ctor for nonDeletedTuple
			:data(data), block(block), nextDeleted(0){}
		Tuple(BufferManager::DataPtr data, TupleOffset nextDeleted, Block* block)
			:data(data), block(block), nextDeleted(nextDeleted){}
		Tuple(Tuple&& other):data(std::move(other.data)), 
			nextDeleted(std::move(other.nextDeleted)), block(std::move(other.block)){}
		const BufferManager::DataPtr GetData(void) const{return data;}
	private:
		BufferManager::DataPtr data;	//point at begin addr of data
		TupleOffset nextDeleted;	//offset to the next deleted tuple offset in the block (used when deleted)
		Block *block;	//the block this tuple locates
};

	struct Head {
	public:
		Head(){}
		Head(size_t number, size_t totalTupleNum, bool terminate, Tuple::TupleOffset firstDeleted)
			:number(number), totalTupleNum(totalTupleNum), terminate(terminate), firstDeleted(firstDeleted){}
		Head(Head&& other)
			:number(other.number), totalTupleNum(other.totalTupleNum), terminate(other.terminate), 
			firstDeleted(std::move(other.firstDeleted)){}
	public:
		size_t number;	//number of this block of the whole relation

		/* number of all the tuples (including deleted and undeleted)
		 * used to calc the remaining space of this block
		 */
		size_t totalTupleNum; 
		bool terminate; //if is the last block
		Tuple::TupleOffset firstDeleted;
	};
	
	struct Block{
	public:
		typedef std::list<Tuple> TupleSet;
		typedef TupleSet::iterator TupleIter;
		typedef TupleSet::const_iterator TupleConstIter;
	public:
		Block(){}
		Block(Head&& head):header(std::move(head)){}
		Block(Block&& other)
			:header(std::move(other.header)), tuples(std::move(other.tuples)){}
	public:
		Head header;
		TupleSet tuples;
	};

}