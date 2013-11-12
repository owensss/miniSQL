#pragma once
#include "DataUnit.hpp"
#include "blockReadWrite.hpp"
#include "RecordRelationManager.hpp"

namespace dataUnit{
	//get a list sorted with num order from the original map
std::list<catalog::Field> get_sorted_field_list(const catalog::MetaRelation::fieldSet& fields){
	std::list<catalog::Field> fieldList;
	std::transform(fields.begin(), fields.end(), std::back_inserter(fieldList), 
		[](const std::pair<std::string, catalog::Field>& val){return val.second;} );
	fieldList.sort([](const catalog::Field& lhs, const catalog::Field& rhs){return lhs.num > rhs.num;});
	return std::move(fieldList);
}


size_t get_field_offset(const std::list<catalog::Field> & fieldList, const catalog::Field *field){
	size_t offset = 0;
	auto& end = fieldList.end();
	for(auto& iter = fieldList.begin(); iter != end && iter->name != field->name; iter++){
		offset += getFieldLen(*iter);
	}
	return offset;
}

	//get dataUnit from buffer by field
DataUnit getFieldDataFromBuffer(const std::list<catalog::Field>& fields, 
									   const catalog::Field& field, BufferManager::DataPtr data){
	size_t offs = get_field_offset(fields, &field);
	auto ptr = offs + data;
	DataUnit dataUnit;
	switch(field.type){
	case catalog::Field::CHARS:
		dataUnit.str = (char *)ptr; //get the address of the str
		break;
	case catalog::Field::FLOAT:
		memcpy(&dataUnit.fl, ptr, sizeof(dataUnit.fl));
		break;
	case catalog::Field::INT:
		memcpy(&dataUnit.integer, ptr, sizeof(dataUnit.integer));
		break;
	}
	return dataUnit;
}
	
typedef BufferManager::DataPtr DataPtr;

DataPtr dataUnitToTuple(const std::string& relationName, std::list<DataUnit>& datas, CatalogManager* catalogManager){
		auto &relationInfo = catalogManager->getRelationInfo(relationName);

		size_t tupleLen = catalog::getTupleLen(catalogManager->getRelationInfo(relationName));
		DataPtr data = new byte[tupleLen];
		DataPtr ptr = data;
		std::list<catalog::Field> fields = get_sorted_field_list(relationInfo.fields); //fields with order
		auto &iter = datas.begin();
		for(auto &field: fields){
			switch(field.type){
			case catalog::Field::CHARS:
				{
					auto str = iter->str;
					auto size = field.char_n;
					blockReadWrite::writeBuffer(ptr, (DataPtr)str, std::min(strlen(str)+1, (size_t) size));
					delete []str; //delete those string
					ptr += size;
				}
				break;
			case catalog::Field::FLOAT:
				ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&(iter->fl), sizeof(iter->fl));
				break;
			case catalog::Field::INT:
				ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&(iter->integer), sizeof(iter->integer));
				break;
			default:
				break;
			}
			++iter; //next dataUnit
		}
		return data;
	}
	
//transfer from tuple to dataUnit
std::list<DataUnit> tupleToDataUnit(const std::string& relationName, const record::Tuple& tuple, CatalogManager *catalogManager){
	auto &relationInfo = catalogManager->getRelationInfo(relationName);
	std::list<catalog::Field> fields = get_sorted_field_list(relationInfo.fields);
	const DataPtr tupleData = tuple.GetData();
	auto ptr = tupleData;
	std::list<DataUnit> dataUnitList;
	for(auto& field : fields){
		switch(field.type){
		case catalog::Field::CHARS:{
				size_t char_n = field.char_n;
				char* str = new char[char_n];//may not end with '\0'
				ptr = blockReadWrite::readBuffer(ptr, (DataPtr)str, char_n);
				dataUnitList.push_back(DataUnit(str)); }
			break;
		case catalog::Field::FLOAT: {
				float fl;
				ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&fl, sizeof(fl));
				dataUnitList.push_back(DataUnit(fl)); }
			break;
		case catalog::Field::INT: {
				int integer;
				ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&integer, sizeof(integer)); }
			break;
		default:
			break;
		}//end of switch
	}//end of for
	return dataUnitList;
}

//get dataUnion from buffer
DataUnit getFieldDataFromBuffer(const std::string& relationName, const std::string& field_name, 
								BufferManager::DataPtr data, CatalogManager *catalogManager){
	auto &relationInfo = catalogManager->getRelationInfo(relationName);
	std::list<catalog::Field> fields = get_sorted_field_list(relationInfo.fields);
	return getFieldDataFromBuffer(fields, relationInfo.fields.at(field_name), data);
}
}
