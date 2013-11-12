#include "CatalogManager.hpp"
#include "fileOperation.hpp"
#include "blockReadWrite.hpp"
#include <vector>

/** directory needed
  */
const std::string CatalogManager::directoryName = "MetaData";
const std::string CatalogManager::indexPath = CatalogManager::directoryName + "\\index\\";
const std::string CatalogManager::relationPath = CatalogManager::directoryName + "\\relation\\";

CatalogManager::CatalogManager(BufferManager *bufferManager):bufferManager(bufferManager)
{
	addAllRelationData();
	addAllIndexData();
}


CatalogManager::~CatalogManager(void)
{
}

namespace{
	typedef BufferManager::DataPtr DataPtr;
/** relation: field_num, primary_key_name
  * field: field_name, field_type , char_n, unique
  */

// as a dataBlock for read & write for field
struct FieldData{
public:
	FieldData(catalog::Field::field_type type, size_t char_n, bool unique, uint32_t num)
		:type(type), char_n(char_n), unique(unique), num(num){}
	FieldData(){}
public:
//	[std::string name;] 
	catalog::Field::field_type type;
	uint8_t char_n;
	bool unique;
	uint32_t num;
};

}

namespace catalog{

size_t getFieldLen(const Field& field){
	switch(field.type){
	case Field::FLOAT:
	case Field::INT:
		return 4;
	case Field::CHARS:
		return field.char_n;
	default:
		//should not occur
		return 0;
	}
}
}

/* storage format: 1. field_num (32bit)  2. primary key name (if null => 0 (8 bit) else arbitrary len string end with '\0')
 *	3. Field Data
 */
void CatalogManager::writeRelationData(const std::string& relation_name){
	const auto& relation = relations.at(relation_name);//get relation handle
	blockReadWrite::BlockData block;
	DataPtr dest = block.data;//pointer to data to write

	{
		uint32_t field_num = relation.fields.size();
		dest = blockReadWrite::writeBuffer(dest, (DataPtr)(&field_num), sizeof(field_num));
	}//field_num

	if(relation.primary_key != nullptr)
		dest = blockReadWrite::writeStringToBuffer(dest, relation.primary_key->name); //primary key
	else
		dest = blockReadWrite::writeBuffer(dest, 0, sizeof(uint8_t)); //primary key is null a byte

	for(const auto& field_map: relation.fields){//for all elem(map) inside fieldset
		auto field = field_map.second;
		dest = blockReadWrite::writeStringToBuffer(dest, field.name);//write name
		FieldData fieldData(field.type, field.char_n, field.unique, field.num);
		dest = blockReadWrite::writeBuffer(dest, (DataPtr)(&fieldData), sizeof(fieldData));//other data
	}//write in fields data

	std::string relationFilePath = relationPath+relation_name;
	bufferManager->write(relationFilePath, block.data);
	bufferManager->writeBack(relationFilePath);//write into the memory
	bufferManager->unlock(relationFilePath);
}

catalog::MetaRelation CatalogManager::readRelationData(const std::string& relation_name){
	const std::string relationFilePath = relationPath+relation_name;
	DataPtr source = bufferManager->read(relationFilePath); //ptr to source data

	catalog::MetaRelation relation;
	relation.name = relation_name;

	uint32_t field_num;
	source = blockReadWrite::readBuffer(source, (DataPtr)&field_num, sizeof(field_num));//get field num
	const std::string &&primary_key = blockReadWrite::readStringFromBuffer(source);
	
	auto &fields = relation.fields;
	for(size_t i = 0; i < field_num; i++){
		catalog::Field field;
		field.name = blockReadWrite::readStringFromBuffer(source);

		FieldData field_struct;
		source = blockReadWrite::readBuffer(source, (DataPtr)&field_struct, sizeof(field_struct));
		
		field.type = field_struct.type;
		field.char_n = field_struct.char_n;
		field.unique = field_struct.unique;
		field.num = field_struct.num;

		fields.insert(std::pair<std::string, catalog::Field>(field.name, field));
	}//read in fields data and add into fieldSet
	
	if(primary_key == "")
		relation.primary_key = nullptr;
	else
		relation.primary_key = &fields.at(primary_key);
	bufferManager->unlock(relationFilePath);
	return relation;
}

/**  indexData: name[del], relation_name, field_name
  */

void CatalogManager::writeIndexData(const std::string& index_name){
	blockReadWrite::BlockData block;
	DataPtr dest = block.data;
	const catalog::IndexInfo& index = indexes.at(index_name);
	dest = blockReadWrite::writeStringToBuffer(dest, index.relation->name); //relation_name
	blockReadWrite::writeStringToBuffer(dest, index.field->name);	//field_name

	const std::string&& path = indexPath+index_name;
	bufferManager->write(path, block.data);//write to buffer
	bufferManager->writeBack(path);
	bufferManager->unlock(path);
}

catalog::IndexInfo CatalogManager::readIndexData(const std::string& index_name){
	const std::string&& indexFilePath = indexPath + index_name;
	DataPtr source = bufferManager->read(indexFilePath);
	const std::string&& relation_name = blockReadWrite::readStringFromBuffer(source); //relation_name
	const std::string&& field_name =  blockReadWrite::readStringFromBuffer(source); //field_name
	bufferManager->unlock(indexFilePath);
	auto & relation = relations.at(relation_name);
	return catalog::IndexInfo(index_name, &relation.fields.at(field_name), &relation);
}

void CatalogManager::addAllRelationData(void){
	const std::vector<std::string>&& fileNames = getFileNames(relationPath.c_str());
	for(const auto& fileName : fileNames){
		catalog::MetaRelation&& relation = readRelationData(fileName);
		relations.insert(std::pair<std::string, catalog::MetaRelation>(fileName, relation));
	}
}

void CatalogManager::addAllIndexData(void){
	const std::vector<std::string>&& fileNames = getFileNames(indexPath.c_str());
	for(const auto& fileName : fileNames){
		auto &&index = readIndexData(fileName);
		indexes.insert(std::pair<std::string, catalog::IndexInfo>(fileName, index));
		relations.at(index.relation->name).indexes.insert((std::pair<std::string, catalog::IndexInfo *>(fileName, &index)));
		//add in the relationd
	}
}