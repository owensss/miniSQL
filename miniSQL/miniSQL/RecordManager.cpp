#include "RecordManager.hpp"
#include "DataUnit.hpp"
#include <algorithm>

const std::string RecordManager::directoryName = "record";

namespace{
	using record::DataPtr;

	/* transfer dataUnionList between an array of data, dataUnitToTuple 
	 * need user to delete
	 */
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

		auto dataUnitList = dataUnit::tupleToDataUnit(
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
			records.push_back(dataUnit::tupleToDataUnit(table_name, *tuple_res.second, catalogManager));
		tuple_res = relationManager.getNextTuple(tuple_res.second); //next tuple
	}
	return records;
}

void RecordManager::insert(const std::string& table_name, std::list<DataUnit>& datas){
	addRelation(table_name);
	DataPtr data = dataUnit::dataUnitToTuple(table_name, datas, catalogManager);
	relations.at(table_name).insertTuple(data);
	delete [] data;
}

std::list<record::DataAndLoc> RecordManager::getAllValueAndLoc(const std::string& table_name, const std::string& fieldName){
	addRelation(table_name);
	std::list<record::DataAndLoc> dataAndLocs;
	auto& relationManager = relations.at(table_name);

	return relationManager.getAllValueAndLoc(fieldName);
}