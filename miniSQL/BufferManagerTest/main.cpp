#include "stdafx.h"
#include "../MiniSQL/BufferManager.hpp"
#include "../miniSQL/CatalogManager.hpp"
#include <iostream>

//#define CREATE_TABLE
//#define READ_TABLE
//#define BUFFER_MANAGER
//#define CREATE_INDEX
//#define READ_INDEX
//#define INTERPRETER
#define API

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
	Tokenizer tk("");
	auto s = tk.tokenize("123. asd");

	for (auto a : s)
		cerr << a.value << endl;

	try {
		// error detection
		// if (! interpreter.interprete("execfile")) return 0;
		// select
		if (! interpreter.interprete("select * from AAA; \
		select * from BBB where CCc = 'ddd' and _z < 123 and _f > 12.12 \
			and as999 = \"12154 1356\" and d2 == 123 and a = 1 and a != 1 and a >= 1 and a <= 1")) return 0;
		// insert
		cout << "====================================\n";
		if (! interpreter.interprete("insert into XXX values ( 12, 13.1231243, \"asad\", 'bbbb')")) return 0;
		// delete
		cout << "====================================\n";
		if (! interpreter.interprete("delete from A ; delete from B where something != 'sa'")) return 0;
		// quit
		// if (! interpreter.interprete("quit;")) return 0;
		// create
		cout << "=====================================\n";
		if (! interpreter.interprete("create table XX (\n a int,\n b float unique,\n c char(28) unique,\n primary key (a)); create index a on a(a)")) return 0;
		// drop
		cout << "=====================================\n";
		if (! interpreter.interprete("drop table xxx; drop index bbb")) return 0;
		// execfile
		cout << "=====================================\n";
		if (! interpreter.interprete("execfile test.txt")) return 0;
	} catch (const unexpected_token_exception& e) {
		cerr << e.where().row << ", " << e.where().col << ": " << e.what() << " '" 
			<< e.where().value << "', expect " << e.expect() << endl;
	} catch (const unexpected_end_of_input_exception& e) {
		cerr << e.where().row << ", " << e.where().col << ": " << e.what() << " after '" << e.where().value << 
			"', expect " << e.expect() << endl;
	} catch (const exception& e) {
		cerr << e.what();
		getchar();
	}
	getchar();
}

#endif