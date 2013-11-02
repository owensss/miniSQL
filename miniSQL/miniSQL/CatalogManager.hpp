#pragma once
#include <string>
#include <map>

namespace catalog{
struct Field{
public:
	enum field_type{INT, CHARS, FLOAT};
public:
	Field(std::string& name, field_type type, bool unique)
		: name(name), type(type), unique(unique){};
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
class CatalogManager
{
public:
	
public:
	CatalogManager(void);
	virtual ~CatalogManager(void);

	/**
	  *	interface for RecordManager
	  */
	void createRelationInfo(const std::string& name, const catalog::MetaRelation::fieldSet& fields, 
		const catalog::Field *primary_key){
			relations.insert(
				std::pair<std::string, catalog::MetaRelation>
				(name, (catalog::MetaRelation(name, fields, primary_key))));
	}
	void dropRelationInfo(const std::string& name){
		relations.erase(name);
	}

	void createIndexInfo(const std::string& index_name, const std::string& relation_name, 
		catalog::Field *field){
			catalog::MetaRelation& relation = relations.at(relation_name);
			indexSet::iterator index_iter = indexes.insert(std::pair<std::string, catalog::IndexInfo>
				(index_name, (catalog::IndexInfo(index_name, field, &relation)))).first;
			relation.indexes.insert(
				std::pair<const std::string, const catalog::IndexInfo*>(index_name, &(*index_iter).second));//insert ref
	}
	void dropIndexInfo(const std::string& index_name){
		((indexes.at(index_name).relation)->indexes).erase(index_name);//remove index ref in relation
		indexes.erase(index_name);
	}

	const catalog::MetaRelation& getRelationInfo(const std::string& name){
		return relations.at(name);
	}
	const catalog::Field& getFieldInfo(const std::string& relation_name, const std::string& field_name){
		return relations.at(relation_name).fields.at(field_name);
	}
	const catalog::IndexInfo& getIndexInfo(const std::string& index_name){
		return indexes.at(index_name);
	}
private:
	typedef std::map<std::string, catalog::IndexInfo> indexSet;
	typedef std::map<std::string, catalog::MetaRelation> relationSet;
/*	typedef std::vector<Field>::iterator field_iter;
	typedef std::vector<Field>::const_iterator field_const_iter;
	typedef std::vector<MetaRelation>::iterator relation_iter;
	typedef std::vector<MetaRelation>::const_iterator relation_const_iter;
	typedef std::vector<IndexInfo>::const_iterator index_const_iter;
	typedef std::vector<IndexInfo>::iterator index_iter;	*/
private:
	
private:
	relationSet relations;
	indexSet indexes;
};