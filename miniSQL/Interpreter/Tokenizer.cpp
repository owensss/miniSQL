#include "Tokenizer.hpp"
#include <algorithm>
#include <iterator>
#include <cctype>
#include <functional>
#include "parse_exception.h"

using namespace std;

#define assign_result(T) \
	result.value = string(start, p); \
	result.type = T;

Tokenizer::tokenlist_type Tokenizer::split(const char* token) const {
	tokenlist_type list;

	const char* stmt_start_pos = token;
	int prev_col = 1;
	int prev_row = 1;
	int row = 1;
	int col = 1;
	const char* iter;

	for (iter = token; *iter != '\0'; ++iter) {
		if (newline[*iter] == true) { // newline
			++row;
			col = 1;
		} else ++col;

		if (sep[*iter] == true) {
			// push statement
			list.push_back(statement_type
				(string(stmt_start_pos, iter), token_enum::Statement, prev_row, prev_col)
			);
			// skip sep
			stmt_start_pos = iter+1;
			prev_col = col;
			prev_row = row;
		}
	}

	if (stmt_start_pos != iter) // get last
		list.push_back(statement_type
			(string(stmt_start_pos, iter), token_enum::Statement, prev_row, prev_col)
		);

	return list;
}

Tokenizer::tokenlist_type Tokenizer::tokenize(const char* input) const {
	auto stmts = std::move(split(input));
	tokenlist_type result;
	
	for (const auto& each : stmts) {
		auto t = do_tokenize(each->c_str(), each.row, each.col);
		result.insert(result.end(), t.begin(), t.end());
	}

	return result;
}

/**
 * split statement into tokens. the rules are:
 *    match "" and '' quoted string
 *    match any char that is not 'a-zA-Z_0-9'
 *    match word: 'a-zA-Z_0-9'+
 *    match int: '0-9'+
 *    match double: '0-9' + '.' '0-9'+
 * the function skips skippers, and increase row count by newline indicator
 * this function does not check separator
 */
auto Tokenizer::do_tokenize(const char* token, int start_row, int start_col) const
	-> tokenlist_type 
{
	if (token == nullptr) return tokenlist_type();
	
	tokenlist_type tokens;

	int row = start_row;
	int col = start_col;

	int wbegin = 0;
	for (const char* iter = token; *iter != '\0';) {
		char c = *iter;
		bool skip_parse = false;
		// skipper should be checked before newline
		if (skipper[c] || newline[c]) { // skipper
			skip_parse = true;
		}

		if (! skip_parse) {
			// parse a token
			token_type tk;
			const char* p = iter;
			// parse tokens
			if (isdigit(c)) p = parseInt(iter, tk);
			else if (isalpha(c) || c == '_') p = parseWord(iter, tk);
			else if (c == '\"') p = parseQuote(iter, tk);
			else if (c == '\'') p = parseSingleQuote(iter, tk);
			else p = parseSymbol(iter, tk);
			// set row & col
			tk.col = col;
			tk.row = row;
			// push to result
			tokens.push_back(tk);
			// set offset
			col += p-iter-1;
			// advance iter
			iter = p;
		} else ++iter;

		if (newline[c]) { // newline
			++row;
			col = 1;
		} else ++col;
	}
	return tokens;
}

const char* Tokenizer::parseInt(const char* start, token& result) const {

	const char* p = start;
	for (; *p != '\0'; ++p) {
		char c = *p;
		
		if (! isdigit(c)) {
			if (c == '.' && *(p+1) != '\0' && *(p+1) >= '0' && *(p+1) <= '9') {
				return parseDouble(start, result);
			}
			else break;
		}
	}

	assign_result(token_enum::Int);

	return p;
}

const char* Tokenizer::parseDouble(const char* start, token& result) const {
	bool dot = false;

	const char* p = start;
	for (; *p != '\0'; ++p) {
		char c = *p;

		if (! dot) { // before dot
			if (! isdigit(c)) {
				if (c == '.') {
					dot = true;
				}
			}
		} else { // after dot
			if (! isdigit(c)) break;
		}
	}

	assign_result(token_enum::Double);

	return p;
}

const char* Tokenizer::parseQuote(const char* start, token& result) const {
	const char* p = start;

	/* left quote */
	if (*start != '\"') return start;
	++p;
	for (; *p != '\0'; ++p) {
		char c = *p;

		if (c == '\"') {
			++p; // make p points to one after-quote
			break;
		} else if (c == '\n') {
			throw multiline_quote_exception
				(token(string(start, p), token_enum::Quote, 0, 0));
		}
	}

	assign_result(token_enum::Quote);

	return p;
}

const char* Tokenizer::parseSingleQuote(const char* start, token& result) const {
	const char* p = start;

	/* left quote */
	if (*start != '\'') return start;
	++p;
	for (; *p != '\0'; ++p) {
		char c = *p;

		if (c == '\'') {
			++p; // make p points to one after-quote
			break;
		} else if (c == '\n') {
			throw multiline_quote_exception
				(token(string(start, p), token_enum::SingleQuote, 0, 0));
		}
	}

	assign_result(token_enum::SingleQuote);

	return p;
}

const char* Tokenizer::parseWord(const char* start, token& result) const {
	const char* p = start;

	for (; *p != '\0'; ++p) {
		char c = *p;

		if (! (isalpha(c) || isdigit(c) || c=='_')) {
			break;
		}
	}
	
	assign_result(token_enum::Word);
	return p;
}

/**
 * only the following symbols are allowed:
 * every other token,
 * ==, >=, <=, <, >, =, !=, ++, --, *=, /=, -=, +=, ^=, &=, |=, ~=, %=
 * ===
 */
const char* Tokenizer::parseSymbol(const char* start, token& result) const {
	const char* p = start;

	/** first */
	switch (*p) {
	case '=':
		++p;
		if (*p == '\0' || *p != '=') {
			assign_result(token_enum::Symbol);
			return p;
		} else { // ==
			++p;
			if (*p == '\0' || *p != '=') {
				assign_result(token_enum::Symbol);
				return p;
			} else { // ===
				++p;
				assign_result(token_enum::Symbol);
				return p;
			}
		}
		break;
	case '>':
	case '<':
	case '!':
	case '*':
	case '/':
	case '\\':
	case '|':
	case '&':
	case '%':
	case '~':
		++p;
		if (*p == '\0' || *p != '=') {
			assign_result(token_enum::Symbol);
			return p;
		} else { // x=
			++p;
			assign_result(token_enum::Symbol);
			return p;
		}
		break;
	case '+':
		++p;
		if (*p == '\0' || (*p != '+' && *p != '=')) {
			assign_result(token_enum::Symbol);
			return p;
		} else { // ++, +=
			++p;
			assign_result(token_enum::Symbol);
			return p;			
		}
		break;
	case '-':
		++p;
		if (*p == '\0' || (*p != '-' && *p != '=')) {
			assign_result(token_enum::Symbol);
			return p;
		} else { // --, -=
			++p;
			assign_result(token_enum::Symbol);
			return p;			
		}
	default:
		++p;
		assign_result(token_enum::Symbol);
		return p;
	}
	
	return nullptr;
}