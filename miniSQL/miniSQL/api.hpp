#ifndef API_HPP
#define API_HPP
#include <memory>
#include "../minisql/RecordManager.hpp"
#include "../Interpreter/Interpreter.hpp"

class CatalogManager;
class BufferManager;
class RecordManager;
class IndexManager;

namespace api {

/**
 * This class provides an interface between user & the inner logic
 * this class interacts with interpreter, pass messages to managers, and 
 * print out response to the user.
 */
class Api {
public:
	// forward typedef
	typedef std::shared_ptr<CatalogManager> CatalogManagerType;
	typedef std::shared_ptr<BufferManager> BufferManagerType;
	typedef std::shared_ptr<RecordManager> RecordManagerType;
	typedef std::shared_ptr<IndexManager> IndexManagerType;
public:
	typedef RecordManager::RecordSet RecordSet;
public:
	Api();
	~Api() { } // do nothing
public:
	void createTable(const std::string& name, const catalog::MetaRelation::fieldSet& fields, 
		const std::string& pri_key);
	void dropTable(const std::string& tablename);
	void createIndex(const std::string& indexname, const std::string& tablename, 
		const std::string& attrname);
	void dropIndex(const std::string& indexname);
	RecordSet selectFrom(const std::string& tablename);
	RecordSet selectFrom(const std::string& tablename, const std::list<detail::Condition>& conditions);
	void insertInto(const std::string& table_name, const std::list<token> datas);
	void deleteFromTable(const std::string& tablename);
	void deleteFromTable(const std::string& tablename, const std::list<detail::Condition> conditions);
	void execfile(const std::string& filename) ;
	void quit();
private:
	BufferManagerType buffer; // buffer shall be initialized before all other memebers
	CatalogManagerType catalog;
	RecordManagerType record;
	IndexManagerType  index;
};

}

#endif // API_HPP