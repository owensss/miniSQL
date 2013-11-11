#include "RecordManager.hpp"

const std::string RecordManager::directoryName = "record";

namespace{
	using record::DataPtr;

	/* transfer dataUnionList between an array of data, dataUnitToTuple 
	 * need user to delete
	 */
	DataPtr dataUnitToTuple(const std::string& relationName, std::list<DataUnit>& datas, CatalogManager* catalogManager){
		auto &relationInfo = catalogManager->getRelationInfo(relationName);

		size_t tupleLen = catalog::getTupleLen(catalogManager->getRelationInfo(relationName));
		DataPtr data = new byte[tupleLen];
		DataPtr ptr = data;
		auto &fields = relationInfo.fields;
		auto &iter = datas.begin();
		for(auto &fieldPair: fields){
			auto &field = fieldPair.second;
			switch(field.type){
			case catalog::Field::CHARS:
				{
					auto str = iter->str;
					auto size = field.char_n;
					blockReadWrite::writeBuffer(ptr, (DataPtr)str, std::min(strlen(str)+1, size));
					delete []str; //delete those string
					ptr += size;
				}
				break;
			case catalog::Field::FLOAT:
				ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&(iter->fl), sizeof(iter->fl));
				break;
			case catalog::Field::INT:
				ptr = blockReadWrite::writeBuffer(ptr, (DataPtr)&(iter->integer), sizeof(iter->integer));
				break;
			default:
				break;
			}
			++iter; //next dataUnit
		}
		return data;
	}

	std::list<DataUnit> tupleToDataUnit(const std::string& relationName, const record::Tuple& tuple, CatalogManager *catalogManager){
		auto &relationInfo = catalogManager->getRelationInfo(relationName);
		auto &fields = relationInfo.fields;
		const DataPtr tupleData = tuple.GetData();
		auto ptr = tupleData;
		std::list<DataUnit> dataUnitList;
		for(auto& fieldPair : fields){
			auto& field = fieldPair.second;

			switch(field.type){
			case catalog::Field::CHARS:{
					size_t char_n = field.char_n;
					char* str = new char[char_n];
					ptr = blockReadWrite::readBuffer(ptr, (DataPtr)str, char_n);
					dataUnitList.push_back(DataUnit(str)); }
				break;
			case catalog::Field::FLOAT: {
					float fl;
					ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&fl, sizeof(fl));
					dataUnitList.push_back(DataUnit(fl)); }
				break;
			case catalog::Field::INT: {
					int integer;
					ptr = blockReadWrite::readBuffer(ptr, (DataPtr)&integer, sizeof(integer)); }
				break;
			default:
				break;
			}//end of switch
		}//end of for
		return dataUnitList;
	}
}


RecordManager::RecordManager(void)
{
}


RecordManager::~RecordManager(void)
{
}

void RecordManager::deleteTuples(const std::string& table_name, const std::list<Condition>& conditions){
	addRelation(table_name);
	auto& relation = relations.at(table_name);
	auto& res = relation.getFirstTuple();
	auto& iter = conditions.begin();
	while(res.first){
		//test condition
	}
}

RecordManager::RecordSet RecordManager::select(const std::string& table_name){
	//TODO
	return RecordSet();
}

void RecordManager::insert(const std::string& table_name, std::list<DataUnit>& datas){
	addRelation(table_name);
	DataPtr data = dataUnitToTuple(table_name, datas, catalogManager);
	relations.at(table_name).insertTuple(data);
	delete [] data;
}
