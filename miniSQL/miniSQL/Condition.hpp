#pragma once
#include <string>
#include "RecordManager.hpp"
#include "DataUnit.hpp"

//condition: what attribute(field) compair with what
class Condition {
public:
	enum compare_type{LT, GT, EQ, NE, LE, GE};
	enum operator_type{CHARS, INT, FLOAT};
	//less than, greater than, equal, not equal,
	//less or equal, greater or equal

	Condition(compare_type comp, const catalog::Field* field, DataUnit val)
		: comp(comp), m_field(field), val(val){}

	virtual ~Condition(){};

	const catalog::Field* getField(void) const{ 
		return m_field;
	}

	bool test(const DataUnit& data) const{
		switch(m_field->type){
		case catalog::Field::CHARS:
			return test(data.str);
		case catalog::Field::FLOAT:
			return test(data.fl);
		case catalog::Field::INT:
			return test(data.integer);
		default:
			return false;
		}
	}

private:
	bool test(float lhs) const { return matchCompType(lhs - val.fl); }

	bool test(int lhs) const { return matchCompType(lhs-val.integer); }

	bool test(const char* lhs) const {
		auto rhs = val.str;
		size_t size = m_field->char_n;
		return matchCompType(strncmp(lhs, rhs, size));
	}
	/** have value val and function test
	*	of different type
	*/
	bool matchCompType(int compRes) const{
		switch (comp) {
			case LT:
				return compRes < 0;
			case GT:
				return compRes > 0;
			case EQ:
				return compRes == 0;
			case NE:
				return compRes != 0;
			case LE:
				return compRes <= 0;
			case GE:
				return compRes >= 0;
			default:
				return false;
		}
	}
private:
	const catalog::Field* m_field;//condition is concerned with some attribute
	compare_type comp;
	DataUnit val;
};