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
#else
			const catalog::Field* pri = &fields.at(primary_key);
		catalog->createRelationInfo(name, fields, pri);
#endif
	}

	void Api::dropTable(const std::string& name) {
		catalog->dropRelationInfo(name);
	}

	void Api::createIndex(const std::string& indexname, const std::string& tablename, 
		const std::string& attrname) {
		
		const auto& field = catalog->getFieldInfo(tablename, attrname);
		catalog->createIndexInfo(indexname, tablename, &field);
		/// TODO
	}

	void Api::dropIndex(const std::string& indexname) {
		catalog->dropIndexInfo(indexname);
	}

	Api::RecordSet Api::selectFrom(const std::string& tablename) {
		/// TODO
#ifdef INTERPRETER_ONLY
		cerr << "select from#" << tablename; 
		return Api::RecordSet();
#else
		return record->select(tablename);
#endif
	}

	Api::RecordSet Api::selectFrom(const std::string& tablename, const list<detail::Condition>& cond) {
		/// TODO
		// return record->select(tablename, cond);
		return Api::RecordSet();
	}

	void Api::insertInto(const std::string& tablename, std::list<token> datas) {
		/// record->insert(tablename, datas);
		/// TODO
	}

	void Api::deleteFromTable(const string& tablename) {
		record->deleteAllTuples(tablename);
		/// TODO
	}

	void Api::deleteFromTable(const string& tablename, const std::list<detail::Condition> cond) {
		// record->deleteTuples(tablename, cond);
		/// TODO
	}

	void Api::execfile(const std::string& filename) {
		Interpreter interpreter;
		interpreter.setAPI(this);

		fstream fs;
		fs.open(filename);

		if (fs.is_open()) {
			std::string all;
			std::string tmp;
			while (fs) {
				getline(fs, tmp);
				all += tmp+"\n";
			}
			interpreter.interprete(all);
		}
	}

	void Api::quit() {
		return ;
	}
}