#include "stdafx.h"
#include "../MiniSQL/BufferManager.hpp"
#include "../miniSQL/CatalogManager.hpp"
#include "../miniSQL/blockReadWrite.hpp"
#include <iostream>
#include <string>

//#define CREATE_TABLE
//#define READ_TABLE
//#define BUFFER_MANAGER
//#define READ_INDEX
//#define INTERPRETER
//#define API
/*
int main(){
	BufferManager bufferManager;
	CatalogManager catalogManager(&bufferManager);
	
	auto &relation = catalogManager.getRelationInfo("student");
//	auto &index = catalogManager.getIndexInfo("stunameidx");
//	catalogManager.createIndexInfo("stunameidx", "student", &relation.fields.at("sname"));


	return 0;
}*/

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

	catalog::Field sno(std::string("sno"), catalog::Field::CHARS, 8, false, 0);
	catalog::Field sname(std::string("sname"), catalog::Field::CHARS, 16, true, 1);
	catalog::Field sage(std::string("sage"), catalog::Field::INT, 0, false, 2);
	catalog::Field sgender(std::string("sgender"), catalog::Field::CHARS, 1, false, 3);

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

#ifdef INTERPRETER 
#include "..\Interpreter\SqlRule.hpp"
#include "..\Interpreter\Tokenizer.hpp"
#include <iostream>

using namespace std;

int main() {
	Tokenizer tk(";");
	auto res = tk.tokenize("select insert where table values , ( ) on index prirmary key unique drop into and delete char int execfile");
	SqlRule rule(&res, res.begin());

	rule.select().insert().where().table().values().comma().lbracket().rbracket().on().index()
		.primary().key().unique().drop().into().and()._delete()._char()._int().execfile();

	
	rule.reset().select();
	getchar();
}
#endif

#ifdef API

#include "..\Interpreter\SqlRule.hpp"
#include "..\Interpreter\Tokenizer.hpp"
#include <iostream>

using namespace std;

int main() {
	Interpreter interpreter;
	try {
		interpreter.interprete("select * from AAA");
	} catch (const exception& e) {
		cerr << e.what();
		getchar();
	}
	getchar();
}

#endif