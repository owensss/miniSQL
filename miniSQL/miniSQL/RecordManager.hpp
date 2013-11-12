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
	typedef std::list<record::RelationManager::TupleIter> SeletionSet; //used for seletion
	typedef std::map<std::string, record::RelationManager> RelationSet;
	typedef std::pair<std::string, record::RelationManager> RelationPair;
	typedef RelationSet::iterator RelationIter;
	typedef RelationSet::const_iterator RelationConstIter;
public:
	RecordManager(void);
	virtual ~RecordManager(void);

	SeletionSet getSeletionSet(const std::string& table_name);//get all tuples of the relation

	RecordSet select(const std::string& table_name){ //select from
		return select(table_name, std::list<Condition>());
	}
	RecordSet select(const std::string& table_name, const std::list<Condition> conditions);	//select from where ...

	void deleteAllTuples(const std::string& table_name) {
		deleteTuples(table_name, std::list<Condition>());
	}	//delete from

	void deleteTuples(const std::string& table_name, const std::list<Condition>& conditions);	//delete from where ...
	void insert(const std::string& table_name, std::list<DataUnit>& datas);

	/* called by index manager, return all the value and location info 
	 * about the attribute of that relation
	 */
	std::list<record::DataAndLoc> getAllValueAndLoc(const std::string& table_name, const std::string& fieldName);
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

