#pragma once
#include "RecordBufferManager.hpp"
#include "CatalogManager.hpp"
#include "Condition.hpp"

/**	used for data transfer
  *	if char(n) then put n DataUnit to the list continuously
  */

union DataUnit{
	char ch;
	int	integer;
	float fl;
};

class RecordManager
{
public:
	typedef std::list<record::Tuple> RecordSet;
public:
	RecordManager(void);
	virtual ~RecordManager(void);

	/**
	  *	@para: if no primary key then pass NULL to primary_key
	  */
	RecordSet createTable(const std::string& tableName, const std::list<Field>& fields, const Field* primary_key);
	void dropTable(const std::string& tableName);
//	void createIndex(const std::string& tableName, const std::string& index_name, const std::string& field_name);
//	void dropIndex(const std::string& indexName);

	RecordSet select(const std::string& table_name);	//select from
	RecordSet select(const std::string& table_name, const std::list<Condition> conditions);	//select from where ...

	RecordSet deleteTuples(const std::string& table_name);	//delete from
	RecordSet deleteTuples(const std::string& table_name, const std::list<Condition> conditions);	//delete from where ...
	RecordSet insert (const std::string& table_name, const std::list<DataUnit> datas);
};

