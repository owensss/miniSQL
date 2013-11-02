#include "CatalogManager.hpp"
#include "readFile.h"
#include <vector>

const std::string CatalogManager::directoryName = "MetaData";
const std::string CatalogManager::indexPath = CatalogManager::directoryName + "\\index\\";
const std::string CatalogManager::relationPath = CatalogManager::directoryName + "\\relation\\";

CatalogManager::CatalogManager(BufferManager *bufferManager):bufferManager(bufferManager)
{
}


CatalogManager::~CatalogManager(void)
{
}

namespace{
typedef BufferManager::DataPtr DataPtr;

/** used to manage a block's memory
  */
struct BlockData{
	BlockData(void){
		data = new byte[BufferManager::BLOCK_SIZE];
	}
	~BlockData(){
		delete [] data;
	}
	DataPtr data;
};

/** as a dataBlock for read & write
  */
typedef catalog::Field FieldData;
struct RelationData{
	RelationData(){};
	RelationData(const std::string& name, size_t field_num, const std::string&  primary_key)
		:name(name), field_num(field_num), primary_key(primary_key){}
	std::string name;
	size_t field_num;
	std::string primary_key;	//if not exists then nullptr
};
}
void CatalogManager::writeRelationData(const std::string& relation_name){
	const auto& relation = relations.at(relation_name);//get relation handle
	BlockData block;
	DataPtr dest = block.data;//pointer to data to write

	memcpy(dest, &RelationData(relation.name, relation.fields.size, relation.primary_key->name), sizeof(RelationData));
	dest += sizeof(RelationData);//write in relation's data

	for(const auto& field: relation.fields){
		memcpy(dest, &field, sizeof(field));
		dest += sizeof(field);
	}//write in fields data

	std::string relationFilePath = relationPath+relation_name;
	bufferManager->write(relationFilePath, block.data);
	bufferManager->writeBack(relationFilePath);
	bufferManager->unlock(relationFilePath);
}

catalog::MetaRelation&& CatalogManager::readRelationData(const std::string& relation_name){
	const std::string relationFilePath = relationPath+relation_name;
	DataPtr block = bufferManager->read(relationFilePath);
	DataPtr source = block;//ptr to source data
	RelationData relationData;
	memcpy(&relationData, source, sizeof(relationData));//fill relationData struct
	source += sizeof(RelationData);

	catalog::MetaRelation::fieldSet fields;
	for(size_t i = 0; i < relationData.field_num; i++){
		FieldData field;
		size_t size = sizeof(field);
		memcpy(&field, source, sizeof(field));
		source += sizeof(field);
		fields.insert(std::pair<std::string, catalog::Field>(field.name, field));
	}//read in fields data and add into fieldSet

	bufferManager->unlock(relationFilePath);
	return catalog::MetaRelation(relationData.name, fields, &fields.at(relationData.primary_key));
}

namespace {
//data w/r between file(buffer)
struct IndexData{
	IndexData(){};
	IndexData(const std::string& name, const std::string& relation_name, const std::string& field_name)
		:name(name), relation_name(relation_name), field_name(field_name){}
	std::string name;
	std::string relation_name;
	std::string field_name;
};
}

void CatalogManager::writeIndexData(const std::string& index_name){
	BlockData block;
	const catalog::IndexInfo& index = indexes.at(index_name);
	memcpy(block.data, &IndexData(index_name, index.relation->name, index.field->name), sizeof(IndexData));
	//write to block

	const std::string&& indexPath = indexPath+index_name;
	bufferManager->write(indexPath, block.data);//write to buffer
	bufferManager->writeBack(indexPath);
	bufferManager->unlock(indexPath);
}

catalog::IndexInfo&& CatalogManager::readIndexData(const std::string& index_name){
	const std::string&& indexFilePath = indexPath + index_name;
	DataPtr block = bufferManager->read(indexFilePath);
	IndexData indexData;
	memcpy(&indexData, block, sizeof(indexData));//read in indexData from memory
	bufferManager->unlock(indexFilePath);

	catalog::MetaRelation &relation = relations.at(indexData.relation_name);//assume relation exist
	//get handle of relation

	return catalog::IndexInfo(index_name, &relation.fields.at(indexData.field_name), &relation);
}

void CatalogManager::addAllRelationData(void){
	const std::vector<std::string>&& fileNames = getFileNames(directoryName);
	for(const auto& fileName : fileNames){
		// addAllRelationData(path)
	}
}