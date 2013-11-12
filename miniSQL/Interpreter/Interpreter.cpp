#include "Interpreter.hpp"
#include "SqlRule.hpp"
#include "parse_exception.h"
#include "..\miniSQL\api.hpp"

using namespace std;

bool Interpreter::interprete(const std::string& input) const {
	auto stmts = split_statement(input);
	for (auto& stmt : stmts) {
		auto tks = tokenize(stmt);
		if (! PaRsE(tks)) return false;
	}

	return true;
}

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

bool Interpreter::PaRsE(const tokenlist_type& tokens) const {
	SqlRule rule(&tokens, tokens.begin());

	if (rule.isEnd()) return true;

	if      (rule.test_select()) parseSelect(tokens, rule);
	else if (rule.test_create()) parseCreate(tokens, rule);
	else if (rule.test_insert()) parseInsert(tokens, rule);
	else if (rule.test_drop())   parseDrop(tokens, rule);
	else if (rule.test_delete()) parseDelete(tokens, rule);
	else if (rule.test_execfile())parseExecfile(tokens, rule);
	else if (rule.test_quit()) {
		parseQuit(tokens, rule);
		return false;
	} else throw unexpected_token_exception(*rule.getIter(), "select, create, insert, delete, drop, execfile, quit");
	
	if (! rule.isEnd()) throw unexpected_token_exception(*rule.getIter(), "end of statement");

	return true;
}

void Interpreter::parseSelect(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename = "";
	rule.select().star().from().parseString(tablename);

	if (rule.isEnd()) api->selectFrom(tablename);
	else if (rule.test_where()) {
		std::list<detail::Condition> conds;
		rule.advance();

		if (rule.isEnd())
			throw unexpected_end_of_input_exception(*--rule.getIter(), "an attribute name");
		// attr_name-operator-value
		while (1) {
			detail::Condition cond;

			rule.parseString(cond.name).parseOperator(cond.op).parseValue(cond.value);
			conds.push_back(cond);
			if (rule.isEnd()) {
				api->selectFrom(tablename, conds);
				break;
			}
			else rule.and();
		}
	} else throw unexpected_token_exception(*rule.getIter(), "'where' or end of input");
}

void Interpreter::parseCreate(const tokenlist_type& tokens, SqlRule& rule) const {
	rule.advance();
	if (rule.isEnd()) throw unexpected_end_of_input_exception(*--rule.getIter(), "'table' or 'index'");
	
	if (rule.test_table()) {
		parseCreateTable(tokens, rule);
	} else if (rule.test_index()) {
		parseCreateIndex(tokens, rule);
	} else
		throw unexpected_token_exception(*rule.getIter(), "'table' or 'index'");
}

void Interpreter::parseCreateTable(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename;
	int num = 0;
	rule.table().parseString(tablename).lbracket();
	typedef catalog::MetaRelation::fieldSet fieldSet;
	typedef catalog::Field Field;
	fieldSet set;

	// attrname-type-[opt]unique-comma loop
	while (1) {
		if (rule.isEnd())
			throw unexpected_end_of_input_exception(*--rule.getIter(), "attribute name or key word 'primary'");
		else if (rule.test_primary()) {
			break; // parse primary key
		} else if (rule.testString()) { // loop
			Field f;
			// attrname
			rule.parseString(f.name);
			if (rule.isEnd())
				throw unexpected_end_of_input_exception(*--rule.getIter(), "a type");
			if (rule.test_int()) {
				f.type = Field::field_type::INT;
				rule.advance();
				if (rule.isEnd())
					throw unexpected_end_of_input_exception(*--rule.getIter(), "comma, or 'unique'");
				f.unique = false;
				if (rule.test_unique()) {
					f.unique = true;
					rule.advance();
				}

				rule.comma();
				f.char_n = 0;
				f.num = num++;
				set.insert(std::make_pair(f.name, f));
			} else if (rule.test_char()) {
				f.type = Field::field_type::CHARS;
				int char_n = 0;

				rule.advance().lbracket().parseInt(char_n).rbracket();
				f.char_n = char_n;

				if (rule.isEnd())
					throw unexpected_end_of_input_exception(*--rule.getIter(), "comma, or 'unique'");
				f.unique = false;
				if (rule.test_unique()) {
					f.unique = true;
					rule.advance();
				}

				rule.comma();
				f.num = num++;
				set.insert(std::make_pair(f.name, f));
			} else if (rule.test_float()) {
				f.type = Field::field_type::FLOAT;
				rule.advance();
				if (rule.isEnd())
					throw unexpected_end_of_input_exception(*--rule.getIter(), "comma, or 'unique'");
				f.unique = false;
				if (rule.test_unique()) {
					f.unique = true;
					rule.advance();
				}

				rule.comma();
				f.char_n = 0;
				f.num = num++;
				set.insert(std::make_pair(f.name, f));
			} else throw unexpected_token_exception(*rule.getIter(), "a type");
		} else throw unexpected_token_exception(*rule.getIter(), "a word, or 'primary'");
	}

	std::string pri_key;
	rule.primary().key().lbracket().parseString(pri_key).rbracket().rbracket();
	api->createTable(tablename, set, pri_key);
}

void Interpreter::parseCreateIndex(const tokenlist_type& tokens, SqlRule& rule) const {
	string indexname;
	string tablename;
	string attrname;
	rule.index().parseString(indexname).on().parseString(tablename).lbracket().parseString(attrname).rbracket();

	api->createIndex(indexname, tablename, attrname);
}

void Interpreter::parseInsert(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename;
	rule.insert().into().parseString(tablename).values().lbracket();

	std::list<token> datas;
	while (1) {
		token t;
		rule.parseValue(t);
		datas.push_back(t);
		if (rule.isEnd())
			throw unexpected_end_of_input_exception(*--rule.getIter(), "',' or ')'");
		if (rule.test_comma()) rule.advance();
		else if (rule.test_rbracket()) {
			rule.rbracket();
			api->insertInto(tablename, datas);
			return;
		} else throw unexpected_token_exception(*rule.getIter(), "expect ',' or ')'");
	}
}

void Interpreter::parseDrop(const tokenlist_type& tokens, SqlRule& rule) const {
	rule.advance();
	if (rule.isEnd())
		throw unexpected_end_of_input_exception(*--rule.getIter(), "'table' or 'index'");

	if (rule.test_table()) parseDropTable(tokens, rule);
	else if (rule.test_index()) parseDropIndex(tokens, rule);
	else throw unexpected_token_exception(*rule.getIter(), "'table' or 'index'");
}

void Interpreter::parseDropTable(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename; 
	rule.table().parseString(tablename);

	api->dropTable(tablename);
}

void Interpreter::parseDropIndex(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string indexname;
	rule.index().parseString(indexname);

	api->dropIndex(indexname);
}

void Interpreter::parseDelete(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string tablename;
	rule._delete().from().parseString(tablename);
	if (rule.test_where()) {
		std::list<detail::Condition> conds;
		rule.advance();

		if (rule.isEnd())
			throw unexpected_end_of_input_exception(*--rule.getIter(), "an attribute name");
		// attr_name-operator-value
		while (1) {
			detail::Condition cond;

			rule.parseString(cond.name).parseOperator(cond.op).parseValue(cond.value);
			conds.push_back(cond);
			if (rule.isEnd()) {
				api->deleteFromTable(tablename, conds);
				break;
			}
			else rule.and();
		}
	} else {
		api->deleteFromTable(tablename);
	}
}

bool Interpreter::parseExecfile(const tokenlist_type& tokens, SqlRule& rule) const {
	std::string filename;
	rule.execfile().parseFilename(filename);

	return api->execfile(filename);
}

void Interpreter::parseQuit(const tokenlist_type& tokens, SqlRule& rule) const {
	api->quit();
}