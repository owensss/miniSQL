#ifndef API_HPP
#define API_HPP
#include <memory>
#include "../minisql/RecordManager.hpp"

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
	RecordSet select(const std::string& tablename);
	RecordSet select(const std::string& tablename, const std::list<Condition>& conditions);
private:
	BufferManagerType buffer; // buffer shall be initialized before all other memebers
	CatalogManagerType catalog;
	RecordManagerType record;
	IndexManagerType  index;
};

}

#endif // API_HPP