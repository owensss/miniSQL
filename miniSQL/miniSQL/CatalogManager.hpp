#pragma once
#include "BufferManager.hpp"
#include <string>
#include <map>

namespace catalog{
struct Field{
public:
	enum field_type{INT, CHARS, FLOAT};
public:
	Field(std::string& name, field_type type, bool unique)
		: name(name), type(type), unique(unique){};
	Field(){}
public:
	std::string name;
	field_type type;
	bool unique;
};

struct IndexInfo{
public:
	IndexInfo(const std::string& index_name, const Field* field, MetaRelation *relation)
		:index_name(index_name), field(field), relation(relation){};
public:
	std::string index_name;
	const Field  *field;
	MetaRelation *relation;
};

struct MetaRelation{
public:
	typedef std::map<std::string, Field> fieldSet;
	typedef std::map<std::string, const IndexInfo*> indexSet;//reference
public:
	MetaRelation(const std::string& name, const fieldSet& fields,
		const Field* primary_key, const indexSet& indexes)
		:name(name), fields(fields), primary_key(primary_key), indexes(indexes){}
	MetaRelation(const std::string& name, const fieldSet& fields, 
		const Field* primary_key):name(name), fields(fields), primary_key(primary_key){}
public:
	std::string name;
	fieldSet fields;
	const Field* primary_key;	//if not exists then nullptr
	indexSet indexes;
};
} // catalog

/** to create Relation or index info, user should first create field by themselves
  * here one index can relate to only one relation, when relation dropped, index also dropped
  */

class CatalogManager
{
public:
	CatalogManager(BufferManager *bufferManager);
	virtual ~CatalogManager(void);

	void createRelationInfo(const std::string& name, const catalog::MetaRelation::fieldSet& fields, 
		const catalog::Field *primary_key){
			relations.insert(
				std::pair<std::string, catalog::MetaRelation>
				(name, (catalog::MetaRelation(name, fields, primary_key))));
		writeRelationData(name);
	}
	void dropRelationInfo(const std::string& name){
		auto& relation = relations.at(name);
		auto indexes = relation.indexes; //a copy of index
		for(auto &index_pair : indexes){//remove all index files of that relation
			dropIndexInfo(index_pair.second->index_name);
		}
		relations.erase(name); //remove relation
		remove((filePath+name).c_str); //delete file
	}

	void createIndexInfo(const std::string& index_name, const std::string& relation_name, 
		catalog::Field *field){
			catalog::MetaRelation& relation = relations.at(relation_name);
			indexSet::iterator index_iter = indexes.insert(std::pair<std::string, catalog::IndexInfo>
				(index_name, (catalog::IndexInfo(index_name, field, &relation)))).first;
			relation.indexes.insert(
				std::pair<const std::string, const catalog::IndexInfo*>(index_name, &(*index_iter).second));//insert ref
		writeIndexData(index_name);
	}
	void dropIndexInfo(const std::string& index_name){
		auto &index = indexes.at(index_name);
		auto &relation = index.relation;
		(relation->indexes).erase(index_name);//remove index ref in relation
		indexes.erase(index_name); //remove index from manager
		remove((filePath+index_name).c_str);//remove file of that index
		writeRelationData(relation->name); //update relation info
	}

	const catalog::MetaRelation& getRelationInfo(const std::string& name) const{
		return relations.at(name);
	}
	const catalog::Field& getFieldInfo(const std::string& relation_name, const std::string& field_name) const{
		return relations.at(relation_name).fields.at(field_name);
	}
	const catalog::IndexInfo& getIndexInfo(const std::string& index_name) const{
		return indexes.at(index_name);
	}
private:
	static const std::string directoryName;
	static const std::string indexPath;
	static const std::string relationPath;
	typedef std::map<std::string, catalog::IndexInfo> indexSet;
	typedef std::map<std::string, catalog::MetaRelation> relationSet;
private:
	/** used when init, should addAllRelationData first
	  * then add all indexData
	  */
	void addAllRelationData(void);
	void addAllIndexData(void);

	/** create a block file for each relation
	  * suppose that no metaData exceeds 4K
	  */

	/** write Relation's name, field number, primary_key number
	  * in to files
	  */
	void writeRelationData(const std::string& relation_name);
	void writeIndexData(const std::string& index_name);


	/**	@return: MetaRelation that contain empty indexSet
	  */
	catalog::MetaRelation&& readRelationData(const std::string& relation_name);

	/** should be assure that relation related to that index exist
	  */
	catalog::IndexInfo&& readIndexData(const std::string& index_name);
private:
	relationSet relations;
	indexSet indexes;
	BufferManager *bufferManager;
};