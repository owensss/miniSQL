#pragma once
#include <string>

/** Usage:
*	if some field compare with some value is true
*/

//condition: what attribute(field) compair with what
class Condition {
public:
	enum compare_type{LT, GT, EQ, NE, LE, GE};
	enum operator_type{CHARS, INT, FLOAT};
	//less than, greater than, equal, not equal,
	//less or equal, greater or equal

	Condition(compare_type comp, const catalog::Field* field)
		: comp(comp), m_field(field){}

	virtual ~Condition(){};

	const catalog::Field* getField(void) const{ 
		return m_field;
	}

	/** have value val and function test
	*	of different type
	*/
protected:
	const catalog::Field* m_field;//condition is concerned with some attribute
	compare_type comp;
};

template <typename T>
class Cond : Condition{
public:
	Cond(T val, compare_type comp, const catalog::Field* field)
		: Condition(comp, field), val(val){}
public:
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
class Cond<std::string> : Condition{
public:
	Cond(std::string& val, compare_type comp, const catalog::Field* field)
		:Condition(comp, field), val(val){}
public:
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
