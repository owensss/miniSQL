#pragma once
#include "CatalogManager.hpp"
#include "BufferManager.hpp"

union DataUnit{
	DataUnit(){}
	DataUnit(char* str):str(str){};
	DataUnit(int integer):integer(integer){};
	DataUnit(float fl):fl(fl){};
	char* str; //ptr to a string end with '\0'
	int	integer;
	float fl;
};


inline DataUnit getFieldDataFromBuffer(const std::list<catalog::Field>& fields, 
									   const catalog::Field& field, BufferManager::DataPtr data){
	size_t offs = get_field_offset(fields, &field);
	auto ptr = offs + data;
	DataUnit dataUnit;
	switch(field.type){
	case catalog::Field::CHARS:
		dataUnit.str = (char *)ptr; //get the address of the str
		break;
	case catalog::Field::FLOAT:
		memcpy(&dataUnit.fl, ptr, sizeof(dataUnit.fl));
		break;
	case catalog::Field::INT:
		memcpy(&dataUnit.integer, ptr, sizeof(dataUnit.integer));
		break;
	}
	return dataUnit;
}
