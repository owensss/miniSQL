#include "Interpreter.hpp"

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

}