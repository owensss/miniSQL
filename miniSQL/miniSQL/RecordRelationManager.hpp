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
		friend class RelationManager;
	public:
		Tuple(DataPtr data, Block* block)
			:deleted(deleted), data(data), block(block){}
		Tuple(Tuple&& other):deleted(deleted), data(std::move(other.data)), block(std::move(other.block)){}
		const DataPtr GetData(void){
			return data;
		}
	private:
		bool deleted;
		DataPtr data;	//point at its begin addr
		Block *block;	//the block this tuple locates
	};

	/** write into the file, head of block
	  */
	struct Head {
	public:
		Head(){}
		Head(size_t number, size_t totalTupleNum, bool terminate)
			:number(number), totalTupleNum(totalTupleNum), terminate(terminate){}
		Head(Head&& other)
			:number(other.number), totalTupleNum(other.totalTupleNum), terminate(other.terminate){}
	public:
		size_t number;	//number of this block of the whole relation
		size_t totalTupleNum; //number of all the tuples
		bool terminate;
	};
	
	struct Block{
	public:
		typedef std::list<std::list<Tuple>::iterator> TupleIterSet;
		typedef std::list<Tuple>::iterator TupleIter;
	public:
		Block(){}
		Block(Block&& other)
			:header(std::move(other.header)), tuples(std::move(other.tuples)), dataBlock(std::move(other.dataBlock)){}
		Block(Head header, TupleIterSet tuples, blockReadWrite::BlockData dataBlock)
			:header(header), tuples(tuples), dataBlock(dataBlock){}
		Head header;
		TupleIterSet tuples;							//the start address of tuples inside dataBlock.data
		blockReadWrite::BlockData dataBlock;	//concerned with the block to write
	};

	/** when there is no such block in the memory, then read it in.
	  * when the blocks are too large, write some blocks at the head into files 
	  * and erase them from this class
	  */
	class RelationManager{
	public:
		typedef std::list<Tuple> TupleSet;
		typedef std::list<Block> BlockSet;
		typedef std::list<Block>::iterator BlockIter;
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
		size_t initRead(void){
			return readBlocks(GREATEST_BLOCKS_CAPACITY/2);
		}

		//name of the relation
		const std::string& getName(void) const{
			return name;
		}

		size_t getTupleLen(void) const{
			return tuple_len;
		}
		
		/* get the first nonDeletedTuple of the relation, if no tuple then return set.end
		 * bool return if iterator exists
		 */
		std::pair<bool, TupleConstIter> getFirstTuple();
		std::pair<bool, TupleConstIter> getNextTuple(TupleConstIter tuple);

		/** remove it from the nonDeleted list
		  * add to the deleted list
		  * write to block
		  */
		void deleteTuple(TupleIter tuple);

		/** @arg: the pointer to the data
		  * will insert tuple to the first deleted tuple's place
		  * if not exists then add to tail
		  * will also add to the nonDeleted tuple list
		  */
		void insertTuple(DataPtr data);

		//delete all the tuples from the relation
		void deleteAll(void);
	private:
		const static size_t GREATEST_BLOCKS_CAPACITY;
	private:
		/** update info of this block to the block data associated
		  */
		void writeBlock(const Block& block);
		/** write block to the file
		  */
		void writeBlockToFile(const Block& block);

		//read block_number blocks or until no such exists
		size_t readBlocks(size_t block_number){
			size_t i = 0;
			for( ; i < block_number && readBlock(i); i++) ;
			return i;
		}
		/** read block from file
		  * @return: whether can get this block
		  */
		bool readBlock(size_t block_no);
		
		/** read and get next block of the tail block
		  * if not exists, then create one
		  */
		void addBlock(void);
		
		/** write block info to files and pop them from the list
		  */
		void releaseBlock(size_t block_no);
		void releaseBlock(const Block& block);
		void releaseBlocks(size_t num);
		void releaseMemory(void);

		//insert block according to the order
		void insertBlock(const Block& block);

		BlockIter findBlockByNumber(size_t no){
			return std::find(blocks.begin(), blocks.end(), 
				[no](const record::Block& block){return block.header.number == no;});
		}
	private:
		BufferManager* bufferManager;
		CatalogManager* catalogManager;

		std::string name;
		size_t tuple_len;	//length of a tuple
		TupleSet deletedTuples;
		TupleSet nondeletedTuples; //for ease of get next tuple operation
		BlockSet blocks; //should in accending order
	};
}
