#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <list>
#include <string>
#include "Tokenizer.hpp"

class SqlRule;
namespace api {
	class Api;
}


namespace detail {
	struct Condition {
		std::string name;
		std::string op;
		token value;
	};
}

class Interpreter {
public:
	typedef Tokenizer::token_type token_type;
	typedef Tokenizer::statement_type statement_type;
	typedef Tokenizer::tokenlist_type statementlist_type;
	typedef Tokenizer::tokenlist_type tokenlist_type;
public:
	Interpreter() :tokenizer(";") {}
	void setAPI(api::Api* api_) { api = api_; }
	api::Api* getAPI() { return api; }
	const api::Api* getAPI() const { return api; }
	/**
	 * entry method that parses string into sql-api
	 *
	 * the entry method, parse the given string, call api interface on finish
	 * if the statement syntax is incorrect, it'll throw error. If multiple statements
	 * are given, it'll parse them all. statements are seperated by ';'
	 * the implementation calls tokenize then calls parse
	 *
	 * if quit, return false; else return true
	 */
	bool interprete(const std::string& str) const;

	/**
	 * split strings into statements and return the statements.
	 * a statement is a set of tokens, seperated by ';', and be parsed by parser
	 */
	statementlist_type split_statement(const std::string& str) const;
	/**
	 * take a single statement as input and tokens as output
	 */
	tokenlist_type tokenize(const statement_type& stmt) const;

	/**
	 * take tokens as input and call sql api as output
	 * if quit, return false, else return true;
	 */
	bool PaRsE(const tokenlist_type& tokens) const;
private:
	void parseCreate(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseCreateTable(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseCreateIndex(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseDrop(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseDropTable(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseDropIndex(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseSelect(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseInsert(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseDelete(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseQuit(const tokenlist_type& tokens, SqlRule& rule) const;
	void parseExecfile(const tokenlist_type& tokens, SqlRule& rule) const;
private:
	Tokenizer tokenizer;
	api::Api* api;
};

#endif // INTER