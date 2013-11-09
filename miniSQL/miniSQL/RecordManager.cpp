#include "RecordManager.hpp"

const std::string RecordManager::directoryName = "record";

RecordManager::RecordManager(void)
{
}


RecordManager::~RecordManager(void)
{
}

RecordManager::RelationIter RecordManager::addRelation(const std::string& relationName){
	auto &relation = relations.insert(RelationPair(relationName, record::RelationManager(relationName, bufferManager, catalogManager)));
	relation.first->second.initRead();
}

void RecordManager::removeRelation(const std::string& relationName){
	relations.erase(relationName);
}

void RecordManager::insert(const std::string& table_name, const std::list<DataUnit>& datas){
	addRelation(table_name);

	size_t tuple_len = 
}

record::Tuple dataUnitToTuple(const std::string& relationName, const std::list<DataUnit> datas){
	
}