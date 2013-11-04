#include "stdafx.h"
#include "../MiniSQL/BufferManager.hpp"
#include "../miniSQL/CatalogManager.hpp"
#include <iostream>

//#define CREATE_TABLE
//#define READ_TABLE
//#define BUFFER_MANAGER
//#define CREATE_INDEX
#define READ_INDEX

#ifdef CREATE_INDEX
int main(){
	BufferManager bufferManager;
	CatalogManager catalogManager(&bufferManager);

	auto &relation = catalogManager.getRelationInfo("student");
	catalogManager.createIndexInfo("stunameidx", "student", &relation.fields.at("sname"));
	return 0;
}
#endif

#ifdef READ_INDEX
int main(){
	BufferManager bufferManager;
	CatalogManager catalogManager(&bufferManager);

	auto &relation = catalogManager.getRelationInfo("student");
	auto &index = catalogManager.getIndexInfo("stunameidx");
	std::cout << "index name: " << index.index_name << std::endl;
	std::cout << "index relation: " << index.relation->name << std::endl;
	std::cout << "index field: " << index.field->name << std::endl;
	return 0;
}
#endif

#ifdef CREATE_TABLE
int main(void){
	BufferManager bufferManager;
	CatalogManager catalogManager(&bufferManager);

	catalog::Field sno(std::string("sno"), catalog::Field::CHARS, 8, false);
	catalog::Field sname(std::string("sname"), catalog::Field::CHARS, 16, true);
	catalog::Field sage(std::string("sage"), catalog::Field::INT, 0, false);
	catalog::Field sgender(std::string("sgender"), catalog::Field::CHARS, 1, false);

	catalog::MetaRelation::fieldSet field_set;
	field_set.insert(std::pair<std::string, catalog::Field> ("sno", sno));
	field_set.insert(std::pair<std::string, catalog::Field> ("sname", sname));
	field_set.insert(std::pair<std::string, catalog::Field> ("sage", sage));
	field_set.insert(std::pair<std::string, catalog::Field> ("sgender", sgender));
	catalogManager.createRelationInfo(std::string("student"), field_set, &sno);
	return 0;
}
#endif

#ifdef READ_TABLE
int main(void){
	BufferManager bufferManager;
	CatalogManager catalogManager(&bufferManager);
	
	const catalog::Field & field = catalogManager.getFieldInfo("student", "sname");
	std::cout << "Field name: " << field.name <<std::endl;
	std::cout << "Field type(0 for int, 1 for chars, 2 for float): " << field.type <<std::endl;
	std::cout << "char_n: " << field.char_n <<std::endl;
	std::cout << "unique: " << field.unique <<std::endl;


	const catalog::MetaRelation & relation = catalogManager.getRelationInfo("student");
	std::cout << "relation name: " << relation.name << std::endl;
	std::cout << "relation primary key: " << relation.primary_key->name << std::endl;
	return 0;
}
#endif

#ifdef BUFFER_MANAGER
int main(){
	BufferManager bufferManager;
	byte* data = bufferManager.read("MetaData\\relation\\student");
	char buffer[100];
	bufferManager.unlock("MetaData\\relation\\student");
}
#endif