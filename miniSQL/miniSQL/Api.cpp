#include "api.hpp"
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include "..\Interpreter\Interpreter.hpp"
#include <list>
#include <fstream>

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
		const catalog::Field *primary_key) {

		catalog->createRelationInfo(name, fields, primary_key);
	}

	void Api::dropTable(const std::string& name) {
		catalog->dropRelationInfo(name);
	}

	void Api::createIndex(const std::string& indexname, const std::string& tablename, 
		const catalog::Field *field) {
			catalog->createIndexInfo(indexname, tablename, field);
	}

	void Api::dropIndex(const std::string& indexname) {
		catalog->dropIndexInfo(indexname);
	}

	Api::RecordSet Api::selectFrom(const std::string& tablename) {
		return record->select(tablename);
	}

	Api::RecordSet Api::selectFrom(const std::string& tablename, const list<Condition>& cond) {
		return record->select(tablename, cond);
	}

	void Api::insertInto(const std::string& tablename, std::list<DataUnit> datas) {
		record->insert(tablename, datas);
	}

	void Api::deleteFromTable(const string& tablename) {
		record->deleteAllTuples(tablename);
	}

	void Api::deleteFromTable(const string& tablename, const std::list<Condition> cond) {
		record->deleteTuples(tablename, cond);
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
}