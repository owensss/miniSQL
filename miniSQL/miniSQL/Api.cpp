#include "api.hpp"
#include "BufferManager.hpp"
#include "CatalogManager.hpp"
#include <list>

using namespace std;

namespace api {
	Api::Api() 
		:buffer(std::make_shared<BufferManager>()) // make sure buffer is created first
	{
		catalog = std::make_shared<CatalogManager>(buffer.get());
		// index = nullptr; // TODO
		record = nullptr; // TODO
	}

	Api::RecordSet Api::select(const std::string& tablename) {
		return record->select(tablename);
	}

	Api::RecordSet Api::select(const std::string& tablename, const list<Condition>& cond) {
		return record->select(tablename, cond);
	}
}