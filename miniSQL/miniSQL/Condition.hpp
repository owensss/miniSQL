#pragma once
#include <string>

/** Usage:
*	if some field compare with some value is true
*/

class Condition {
public:
	enum compare_type{LT, GT, EQ, NE, LE, GE};
	enum operator_type{CHARS, INT, FLOAT};
	//less than, greater than, equal, not equal,
	//less or equal, greater or equal

	Condition(compare_type comp, const Field& field)
		: comp(comp), m_field(field){}

	virtual ~Condition(){};

	Field getField(void) const{
		return m_field;
	}

	/** have value val and function test
	*	of different type
	*/
protected:
	const Field m_field;
	compare_type comp;
};

template <typename T>
class cond : Condition{
public:
	cond(T val, compare_type comp, const Field* field){}
	bool test(T lhs){
		switch (comp) {
		case LT:
			return lhs < val;
		case GT:
			return lhs > val;
		case EQ:
			return lhs == val;
		case NE:
			return lhs != val;
		case LE:
			return lhs <= val;
		case GE:
			return lhs >= val;		
		}
	}
private:
	T val;
};

template <>
class cond<std::string> : Condition{
public:
	cond(std::string& val, compare_type comp, const Field& field):Condition(comp, field){}

	bool test(std::string lhs){
		switch (comp) {
		case LT:
			return lhs.compare(val) < 0;
		case GT:
			return lhs.compare(val) > 0;
		case EQ:
			return lhs.compare(val) == 0;
		case NE:
			return lhs.compare(val) != 0;
		case LE:
			return lhs.compare(val) <= 0;
		case GE:
			return lhs.compare(val) >= 0;
		}
	}
private:
	std::string val;
};
