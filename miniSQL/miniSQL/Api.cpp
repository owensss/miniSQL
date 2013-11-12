#include "api.hpp"
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include "..\Interpreter\Interpreter.hpp"
#include <list>
#include <fstream>
#define INTERPRETER_ONLY

#ifdef INTERPRETER_ONLY
#include <iostream>
#endif

using namespace std;

namespace api {
	Api::Api() 
		:buffer(std::make_shared<BufferManager>()) // make sure buffer is created first
	{
		catalog = std::make_shared<CatalogManager>(buffer.get());
		// index = nullptr; // TODO
		record = nullptr; // TODO
	}

	void Api::createTable(const std::string& name, const catalog::MetaRelation::fieldSet& fields, 
		const std::string& primary_key) {
#ifdef INTERPRETER_ONLY
			cerr << "create table: " << name << endl;
			cerr << "name\tnum\tchar_n\ttype\tunique\n";
			for (auto& field : fields)
				cerr << field.second.name << "\t"<< 
				field.second.num << "\t" << 
				int(field.second.char_n) << "\t" << 
				field.second.type << "\t" << 
				field.second.unique << endl;
			cerr << "primary key: " << primary_key << endl;
#else
			const catalog::Field* pri = &fields.at(primary_key);
		catalog->createRelationInfo(name, fields, pri);
#endif
	}

	void Api::dropTable(const std::string& name) {
#ifdef INTERPRETER_ONLY
		cerr << "drop table: " << name << endl;
#else
		catalog->dropRelationInfo(name);
#endif
	}

	void Api::createIndex(const std::string& indexname, const std::string& tablename, 
		const std::string& attrname) {
#ifdef INTERPRETER_ONLY
		cout << "create Index: indexname: " << indexname << ", tablename, " << tablename << ", attrname" << attrname << endl;
#else
		const auto& field = catalog->getFieldInfo(tablename, attrname);
		catalog->createIndexInfo(indexname, tablename, &field);
		/// TODO
#endif
	}

	void Api::dropIndex(const std::string& indexname) {
#ifdef INTERPRETER_ONLY
		cerr << "drop index: " << indexname << endl;
#else
		catalog->dropIndexInfo(indexname);
#endif
	}

	Api::RecordSet Api::selectFrom(const std::string& tablename) {
		/// TODO
#ifdef INTERPRETER_ONLY
		cerr << "select from: " << tablename << endl; 
		return Api::RecordSet();
#else
		return record->select(tablename);
#endif
	}

	Api::RecordSet Api::selectFrom(const std::string& tablename, const list<detail::Condition>& cond) {
		/// TODO
		// return record->select(tablename, cond);
#ifdef INTERPRETER_ONLY
		cerr << "select from#" << tablename << endl;
		for (auto& c : cond) {
			cerr << "attrname: " << c.name << ", operator: " << c.op << ", value: " << c.value.value << endl;
		}
		return Api::RecordSet();
#else
		// TODO
#endif
	}

	void Api::insertInto(const std::string& tablename, std::list<token> datas) {
		/// record->insert(tablename, datas);
		/// TODO
#ifdef INTERPRETER_ONLY
		cerr << "insertInto: " << tablename << endl;
		for (const auto& data : datas) {
			cerr << data.value << "\t";
		}
		cerr << endl;
#else
#endif
	}

	void Api::deleteFromTable(const string& tablename) {
#ifdef INTERPRETER_ONLY
		cerr << "delete all : " << tablename << endl;
#else
		record->deleteAllTuples(tablename);
#endif
		/// TODO
	}

	void Api::deleteFromTable(const string& tablename, const std::list<detail::Condition> cond) {
#ifdef INTERPRETER_ONLY
	cerr << "delete : " << tablename << endl;
	for (const auto& c : cond)
		cerr << "attrname: " << c.name << ", operator: " << c.op << ", value: " << c.value.value << endl;
#else
		// record->deleteTuples(tablename, cond);
		/// TODO
#endif
	}

	bool Api::execfile(const std::string& filename) {
		Interpreter interpreter;
		interpreter.setAPI(this);

		fstream fs;
		fs.open(filename, std::fstream::in);

		if (fs.is_open()) {
			std::string all;
			std::string tmp;
			while (fs) {
				getline(fs, tmp);
				all += tmp+"\n";
			}
			if (interpreter.interprete(all) == false) return false;
		} else throw ios_base::failure(("unable to open file "+filename).c_str());
		return true;
	}

	void Api::quit() {
		return ;
	}
}