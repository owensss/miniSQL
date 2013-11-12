#include "..\Interpreter\Interpreter.hpp"
#include "../MiniSQL/BufferManager.hpp"
#include "../miniSQL/CatalogManager.hpp"
#include "..\Interpreter\SqlRule.hpp"
#include "..\Interpreter\Tokenizer.hpp"
#include "api.hpp"
#include <iostream>

using namespace std;

int main() {
	Interpreter interpreter;
	// api::Api api;
	// interpreter.setAPI(&api);
	Tokenizer tokenizer(";");
	cout << ">> ";
	while (1) {
		std::string input;

		getline(cin, input); 

		auto tk = tokenizer.onlineTokenize((input+'\n').c_str());
		if (tk.size() != 0) {
			for (const auto& each : tk) {
				const auto& t = interpreter.tokenize(each);
				if (t.size() != 0) {
					cout << "## ";
					try {
						if (! interpreter.PaRsE(t)) return 0;
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
					cout << endl;
				}
			}
			cout << ">> ";
		} else {
			cout << ">  ";
		}
	}
}


/*
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
*/