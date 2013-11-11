#pragma once
#include "CatalogManager.hpp"
#include "RecordRelationManager.hpp"
#include "Condition.hpp"

/**	used for data transfer
  *	if char(n) then put n DataUnit to the list continuously
  */

class RecordManager
{
public:
	static const std::string directoryName;
	typedef std::list<std::list<DataUnit>> RecordSet;
	typedef std::map<std::string, record::RelationManager> RelationSet;
	typedef std::pair<std::string, record::RelationManager> RelationPair;
	typedef RelationSet::iterator RelationIter;
	typedef RelationSet::const_iterator RelationConstIter;
public:
	RecordManager(void);
	virtual ~RecordManager(void);

	RecordSet select(const std::string& table_name);	//select from
	RecordSet select(const std::string& table_name, const std::list<Condition> conditions);	//select from where ...

	void deleteAllTuples(const std::string& table_name) {
		addRelation(table_name)->second.deleteAll();
	}	//delete from

	void deleteTuples(const std::string& table_name, const std::list<Condition>& conditions);	//delete from where ...
	void insert(const std::string& table_name, std::list<DataUnit>& datas);
private:
	typedef record::DataPtr DataPtr;
private:
	//add and init a relationManager and add to relationSet
	RelationIter addRelation(const std::string& relationName){
		return relations.insert(
			RelationPair(relationName, record::RelationManager(relationName, bufferManager, catalogManager)))
			.first;
	}

	//write and remove the relation from memory
	void removeRelation(const std::string& relationName){ relations.erase(relationName); }
	
	
//	DataPtr dataUnitToTuple(const std::string& relationName, const std::list<DataUnit> datas);
private:
	BufferManager *bufferManager;
	CatalogManager *catalogManager;
	RelationSet relations;
};

