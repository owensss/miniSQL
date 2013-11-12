#include "RecordManager.hpp"
#include <algorithm>

const std::string RecordManager::directoryName = "record";

namespace{
	using record::DataPtr;

	/* transfer dataUnionList between an array of data, dataUnitToTuple 
	 * need user to delete
	 */
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
}


RecordManager::RecordManager(void)
{
}


RecordManager::~RecordManager(void)
{
}

namespace {
	//test if all conditions satisfy
	bool testConditions(const std::list<Condition>& conditions, const record::Tuple& tuple,
		const std::string& relation_name, CatalogManager *catalogManager){
		if(conditions.empty()) //if no condition return true
			return true;

		auto dataUnitList = tupleToDataUnit(
			relation_name, tuple, catalogManager);

		auto& iter = conditions.begin();
		auto& end = conditions.end();
		for(	; iter != end; iter++){
			auto field = iter->getField();
			auto &dataUnitIter = dataUnitList.begin();
			std::advance(dataUnitIter, field->num);
			DataUnit data = *dataUnitIter;
			if(!iter->test(data))
				return false;
		}

		return iter == conditions.end();
		//pass all test
	}
}

void RecordManager::deleteTuples(const std::string& table_name, const std::list<Condition>& conditions){
	addRelation(table_name);
	auto& relationManager = relations.at(table_name);

	auto tuple_res = relationManager.getFirstTuple();
	while(tuple_res.first){//get tuple
		if(testConditions(conditions, *tuple_res.second, table_name, catalogManager))
			relationManager.deleteTuple(tuple_res.second);
		tuple_res = relationManager.getNextTuple(tuple_res.second); //next tuple
	}
}

RecordManager::RecordSet RecordManager::select(const std::string& table_name, const std::list<Condition> conditions){
	RecordSet records;
	addRelation(table_name);
	auto& relationManager = relations.at(table_name);

	auto tuple_res = relationManager.getFirstTuple();
	while(tuple_res.first){//get tuple
		if(testConditions(conditions, *tuple_res.second, table_name, catalogManager))
			records.push_back(tupleToDataUnit(table_name, *tuple_res.second, catalogManager));
		tuple_res = relationManager.getNextTuple(tuple_res.second); //next tuple
	}
	return records;
}

void RecordManager::insert(const std::string& table_name, std::list<DataUnit>& datas){
	addRelation(table_name);
	DataPtr data = dataUnitToTuple(table_name, datas, catalogManager);
	relations.at(table_name).insertTuple(data);
	delete [] data;
}
