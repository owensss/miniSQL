#pragma once
#include "BufferManager.hpp"
#include "common_exception.hpp"
#include <string>
#include <map>
#include <algorithm>
#include <list>

namespace catalog{
struct Field{
public:
	enum field_type{INT, CHARS, FLOAT};
public://if not chars then pass 0 to char_n
	Field(std::string& name, field_type type, size_t char_n, bool unique, size_t num)
		: name(name), type(type), char_n(char_n), unique(unique), num(num){};
	Field(){}
public:
	std::string name;
	field_type type;
	uint8_t char_n; //not consider '\0', stored also as char_n bytes
	bool unique;
	uint32_t num;	//order number in the catalog
};

//return the len of the field
size_t getFieldLen(const Field& field);

struct MetaRelation;
struct IndexInfo{
public:
	IndexInfo(const std::string& index_name, const Field* field, MetaRelation *relation)
		:index_name(index_name), field(field), relation(relation){};
	IndexInfo(IndexInfo && other)
		:index_name(std::move(other.index_name)), field(std::move(other.field)), relation(std::move(other.relation)){}
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
	MetaRelation(){};
	MetaRelation(const std::string& name, const fieldSet& fields,
		const Field* primary_key, const indexSet& indexes)
		:name(name), fields(fields), primary_key(primary_key), indexes(indexes){}
	MetaRelation(const std::string& name, const fieldSet& fields, 
		const Field* primary_key):name(name), fields(fields), primary_key(primary_key){}
	MetaRelation(const MetaRelation& other):
		name(other.name), fields(other.fields), indexes(other.indexes){
			if(other.primary_key == nullptr)
				this->primary_key = nullptr;
			else
				this->primary_key = &this->fields.at(other.primary_key->name);
	}

	MetaRelation(MetaRelation&& other)
		:name(std::move(other.name)), fields(std::move(other.fields)), 
		primary_key(std::move(other.primary_key)), indexes(std::move(other.indexes)){}
public:
	std::string name;
	fieldSet fields;
	const Field* primary_key;	//if not exists then nullptr
	indexSet indexes;
};

inline size_t getTupleLen(const MetaRelation& relation){
	size_t len = 0;
	for(auto field_map: relation.fields){
		len += getFieldLen(field_map.second);
	}
	return len;
}

//get a list sorted with num order from the original map
extern std::list<Field> get_sorted_field_list(const MetaRelation::fieldSet& fields);
extern size_t get_field_offset(const std::list<Field> & fieldList, const Field *field);
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
		auto res =	relations.insert(
				std::pair<std::string, catalog::MetaRelation>
				(name, (catalog::MetaRelation(name, fields, primary_key))));
		writeRelationData(name);
		if(!res.second)//already exists
			throw duplicate(name);
	}
	void dropRelationInfo(const std::string& name){
		try{
			auto& relation = relations.at(name);
			auto indexes = relation.indexes; //a copy of index
			for(auto &index_pair : indexes){//remove all index files of that relation
				dropIndexInfo(index_pair.second->index_name);
			}
			relations.erase(name); //remove relation
			remove((directoryName+name).c_str()); //delete file
		}catch(std::out_of_range e){
			throw not_exists(name);}
	}

	void createIndexInfo(const std::string& index_name, const std::string& relation_name, 
		const catalog::Field *field){
		try{
			catalog::MetaRelation& relation = relations.at(relation_name);
			indexSet::iterator index_iter = indexes.insert(std::pair<std::string, catalog::IndexInfo>
				(index_name, (catalog::IndexInfo(index_name, field, &relation)))).first;
			auto res = relation.indexes.insert(
				std::pair<const std::string, const catalog::IndexInfo*>(index_name, &(*index_iter).second));//insert ref
			if(!res.second){
				throw duplicate(index_name);
			}
			writeIndexData(index_name);
		}catch(std::out_of_range e){
			throw not_exists(relation_name);
		}
	}
	void dropIndexInfo(const std::string& index_name){
		try{
		auto &index = indexes.at(index_name);
		auto &relation = index.relation;
		(relation->indexes).erase(index_name);//remove index ref in relation
		indexes.erase(index_name); //remove index from manager
		remove((directoryName+index_name).c_str());//remove file of that index
		writeRelationData(relation->name); //update relation info
		}catch (std::out_of_range e){
			throw not_exists(index_name);
		}
	}

	const catalog::MetaRelation& getRelationInfo(const std::string& name) const{
		try{
			return relations.at(name);
		}catch(std::out_of_range e){
			throw not_exists(name);
		}
	}
	const catalog::Field& getFieldInfo(const std::string& relation_name, const std::string& field_name) const{
		try{
		return getRelationInfo(relation_name).fields.at(field_name);
		}catch(std::out_of_range e){
			throw not_exists(field_name);
		}
	}
	const catalog::IndexInfo& getIndexInfo(const std::string& index_name) const{
		try{
			return indexes.at(index_name);
		}catch(std::out_of_range e){
			throw not_exists(index_name);
		}
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
	catalog::MetaRelation readRelationData(const std::string& relation_name);

	/** should be assure that relation related to that index exist
	  */
	catalog::IndexInfo readIndexData(const std::string& index_name);
private:
	relationSet relations;
	indexSet indexes;
	BufferManager *bufferManager;
};