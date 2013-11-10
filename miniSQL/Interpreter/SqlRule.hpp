#ifndef SQL_RULE_HPP
#define SQL_RULE_HPP
#include "Interpreter.hpp"

/**
 * this class accept token list as input, offers
 * a set of 'match' functions = =b
 * and a record to indicate match result orz...
 */
class SqlRule {
public:
	typedef Interpreter::token_type token_type;
	typedef Interpreter::tokenlist_type tokenlist_type;
	typedef tokenlist_type::const_iterator iterator;
public:
	SqlRule(bool match, const tokenlist_type* tk, iterator iter) :match(match), tokens(tk), iter(iter) {}
	SqlRule() { match = true; tokens = nullptr; }
	~SqlRule() {}

	void clear() { tokens = nullptr; match = true; }
	void resetIter() {iter = tokens->begin(); end = tokens->end();}
	void set(tokenlist_type* tks) {
		tokens = tks;
		resetIter();
	}
	SqlRule& rematch() { match = true; return *this;}
	SqlRule& reset() { match = true; resetIter(); return *this; }
public:
	bool ismatch() const { return match; }
	iterator getMismatch() const { return fail; }
	operator bool() const {
		return match;
	}
	SqlRule& select() {
		if (iter == end || iter->value != "select") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;
	}
	SqlRule& insert() {
		if (iter == end || iter->value != "insert") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;
	}
	SqlRule& values() {
		if (iter == end || iter->value != "values") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& create() {
		if (iter == end || iter->value != "create") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& table() {
		if (iter == end || iter->value != "table") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& prirmary() {
		if (iter == end || iter->value != "prirmary") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& key() {
		if (iter == end || iter->value != "key") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& lbracket() {
		if (iter == end || iter->value != "(") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& rbracket() {
		if (iter == end || iter->value != ")") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& comma() {
		if (iter == end || iter->value != ",") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& _char() {
		if (iter == end || iter->value != "char") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& _int() {
		if (iter == end || iter->value != "int") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& unique() {
		if (iter == end || iter->value != "unique") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& drop() {
		if (iter == end || iter->value != "drop") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& on() {
		if (iter == end || iter->value != "on") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& index() {
		if (iter == end || iter->value != "index") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& from() {
		if (iter == end || iter->value != "from") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& where() {
		if (iter == end || iter->value != "where") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& and() {
		if (iter == end || iter->value != "and") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& into() {
		if (iter == end || iter->value != "into") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& _delete() {
		if (iter == end || iter->value != "delete") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;		
	}
	SqlRule& execfile() {
		if (iter == end || iter->value != "execfile") { if (match) {match = false; fail = iter;} }
		++iter;
		return *this;
	}
private:
	// current iter
	iterator iter;
	iterator fail;
	iterator end;
	const tokenlist_type* tokens;
	bool match;
};

#endif // SQL_RULE_HPP