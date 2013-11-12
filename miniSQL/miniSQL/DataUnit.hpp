#pragma once
#include "CatalogManager.hpp"
#include "BufferManager.hpp"

namespace record {
	class Tuple;
}

union DataUnit{
	DataUnit(){}
	DataUnit(char* str):str(str){};
	DataUnit(int integer):integer(integer){};
	DataUnit(float fl):fl(fl){};
	char* str; //ptr to a string end with '\0'
	int	integer;
	float fl;
};

namespace dataUnit{
	extern DataUnit getFieldDataFromBuffer(const std::string& relationName, const std::string& field_name, 
								BufferManager::DataPtr data, CatalogManager *catalogManager);

	extern DataUnit getFieldDataFromBuffer(const std::list<catalog::Field>& fields, 
									   const catalog::Field& field, BufferManager::DataPtr data);
	extern BufferManager::DataPtr dataUnitToTuple(const std::string& relationName, std::list<DataUnit>& datas, CatalogManager* catalogManager);

	//transfer from tuple to dataUnit
	extern std::list<DataUnit> tupleToDataUnit
		(const std::string& relationName, const record::Tuple & tuple, CatalogManager *catalogManager);
}
