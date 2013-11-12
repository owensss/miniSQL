#pragma once
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include "blockReadWrite.hpp"
#include <list>
#include <iterator>

namespace record{
	typedef BufferManager::DataPtr DataPtr;

	struct Block;
	class RelationManager;
	class Tuple{
		friend class RelationManager; //can be accessed by relationManager
	public:
		typedef size_t TupleOffset;
		const static size_t NULL_OFFSET;// 0xFFFF
	public:
		Tuple(DataPtr data, Block* block) //ctor for nonDeletedTuple
			:data(data), block(block), nextDeleted(0){}
		Tuple(DataPtr data, TupleOffset nextDeleted, Block* block)
			:data(data), block(block), nextDeleted(nextDeleted){}
		Tuple(Tuple&& other):data(std::move(other.data)), 
			nextDeleted(std::move(other.nextDeleted)), block(std::move(other.block)){}
		const DataPtr GetData(void) const{return data;}
	private:
		DataPtr data;	//point at begin addr of data
		TupleOffset nextDeleted;	//offset to the next deleted tuple offset in the block (used when deleted)
		Block *block;	//the block this tuple locates
	};

	/** write into the file, head of block
	  */
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
		TupleSet tuples;	//the start address of tuples inside dataBlock.data
	};

	/** when there is no such block in the memory, then read it in.
	  * when the blocks are too large, write some blocks at the head into files 
	  * and erase them from this class
	  */
	class RelationManager{
	public:
		typedef std::map<size_t, Block> BlockSet;
		typedef std::pair<size_t, Block> BlockPair;
		typedef BlockSet::iterator BlockIter;

		typedef std::list<Tuple> TupleSet;
	 	typedef TupleSet::iterator TupleIter;
		typedef TupleSet::const_iterator TupleConstIter;
	public:
		RelationManager(const std::string& name, BufferManager* bufferManager, CatalogManager* catalogManager)
			:name(name), bufferManager(bufferManager), catalogManager(catalogManager){
				tuple_len = catalog::getTupleLen(catalogManager->getRelationInfo(name));
		}
		RelationManager(RelationManager&& other):name(std::move(other.name)), bufferManager(std::move(other.bufferManager)),
			catalogManager(std::move(other.catalogManager)), tuple_len(other.tuple_len){}
		~RelationManager(); //write all info to files

		const std::string& getName(void) const{ return name; } //get the name of the relation
		size_t getTupleLen(void) const{ return tuple_len; } // get the length of the tuple
		
		/* get the first nonDeletedTuple of the relation, if no tuple then return set.end
		 * bool return if iterator exists
		 */
		std::pair<bool, TupleIter> getFirstTuple(size_t block_num); // get first tuple of block_n
		std::pair<bool, TupleIter> getFirstTuple(){ return getFirstTuple(0); }	// get first tuple of block_0
		std::pair<bool, TupleIter> getNextTuple(TupleIter tuple);

		void deleteTuple(TupleIter tuple); // add tuple to the free list and remove from the block

		// find the first position in the free list to insert from the current first block in the memory
		bool insertTuple(DataPtr data); 

		// delete all record files of relation
		void deleteAll(void);
	private:
		/*	a standard that block num in the memory shouldn't exceed
			if exceeds then some blocks shall be released
		 */
		const static size_t GREATEST_BLOCKS_CAPACITY; 
	private:

		/* if block in the blockSet, just return that
		 * else read in the block from disk
		 * user should assure that the block is exists in the disk
		 */
		record::Block& getBlock(size_t block_num){
			auto iter = blocks.find(block_num);
			if(iter == blocks.end())  //if not exists then read
				readBlock(block_num);
			return blocks.at(block_num);
		}

		/** write block to the file
		  * create a dataBlock and write in it and then write that block to file
		  */
		void writeBlock(const Block& block);

		//read block_number blocks or until no such exists
/*		size_t readBlocks(size_t block_number){
			size_t i = 0;
			for( ; i < block_number && readBlock(i); i++) ;
			return i;
		}*/

		/** read in already existed block from file
		  * @return: whether can get this block (may not exists)
		  */
		bool readBlock(size_t block_no);
		
		// create a new block and read into memory
		void appendBlock(void);
		
		//write to files and removed from memory
		void releaseBlock(size_t block_no);
		void releaseBlock(const Block& block){ releaseBlock(block.header.number); }

		//release blocks from the earliest one
		void releaseBlocks(size_t num);

		void RelationManager::releaseMemory(void);//release memory if exceeds

		bool doInsertTuple(Block& block, DataPtr data);
/*		BlockIter findBlockByNumber(size_t no){
			return std::find(blocks.begin(), blocks.end(), 
				[no](const record::Block& block){return block.header.number == no;});
		}*/
	private:
		BufferManager* bufferManager;
		CatalogManager* catalogManager;

		std::string name; //relation name
		size_t tuple_len;	//length of a tuple
		BlockSet blocks;
	};
}
