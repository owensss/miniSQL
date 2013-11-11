#include "Interpreter.hpp"
#include "SqlRule.hpp"
#include "parse_exception.h"
#include "..\miniSQL\api.hpp"

/**
 * split strings into statements and return the statements.
 * a statement is a set of tokens, seperated by ';', and be parsed by parser
 */

Interpreter::statementlist_type Interpreter::split_statement(const std::string& str) const {
	return tokenizer.split(str.c_str());
}

Interpreter::tokenlist_type Interpreter::tokenize(const statement_type& stmt) const {
	return tokenizer.tokenize(stmt);
}

void Interpreter::PaRsE(const tokenlist_type& tokens) const {
	SqlRule rule(&tokens, tokens.begin());
	
	if      (rule.test_select()) parseSelect(tokens, rule);
	else if (rule.test_create()) parseCreate(tokens, rule);
	else if (rule.test_insert()) parseInsert(tokens, rule);
	else if (rule.test_drop())   parseDrop(tokens, rule);
	else if (rule.test_delete()) parseDelete(tokens, rule);
	else if (rule.test_execfile())parseExecfile(tokens, rule);
	else if (rule.test_quit())   parseQuit(tokens, rule);
}

void Interpreter::parseSelect(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename;
	rule.select().star().from().parseString(tablename);
	if (rule.test_where()) {
	} else {
		api->selectFrom(tablename);
	}
}

void Interpreter::parseInsert(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename;
	rule.insert().into().parseString(tablename).values().lbracket();
}

void Interpreter::parseDelete(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename;
	rule._delete().from().parseString(tablename);
	if (rule.test_where()) {

	} else {
		api->deleteFromTable(tablename);
	}
}

void Interpreter::parseCreate(const tokenlist_type& tokens, SqlRule& rule) const {
	std::
}