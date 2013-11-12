#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <string>
#include <list>
#include "detail\char_hash.h"

/**
 * this class enumerates all types tokenizer recognized
 */

enum class token_enum {Int, Double, Word, Symbol, Quote, SingleQuote, Statement};

/**
 * the token type, which holds value as well as col & row of that token
 * this class only works for Tokenizer.
 */
struct token {
	typedef std::string value_type;

	value_type value;
	token_enum type;
	int row;
	int col;

	/**
	 * a dereference operator converting token to value
	 */
	value_type* operator ->() {
		return &value;
	}

	const value_type* operator ->() const {
		return &value;
	}

	token(value_type value, token_enum type, int row, int col)
		:value(value), type(type), row(row), col(col) {}
	token() {}
};

/**
 * a simple tokenizer class, separate 
 */
class Tokenizer {
public:
	typedef token statement_type; // a statement is also defined as a token
	typedef token token_type;
	typedef std::list<token_type> tokenlist_type;
public:
	/**
	 * construct tokenizer by separaters skipper, and newline indicator.
	 * the separater shall not share same chars with skipper or newline indicator.
	 * this tokenizer , as a simple one, obeys the following rules:
	 *    match "" and '' quoted string
	 *    match any char that is not 'a-zA-Z_0-9'
	 *    match word: 'a-zA-Z_0-9'+
	 *    match int: '0-9'+
	 *    match double: '0-9' + '.' '0-9'+
	 *
	 * @param separaters splits string into statements. (e.g ";-" makes both ';' and '-' as separater.)
	 * @param skipper blanks to skip, default to " \t\n" (i.e, skip all blanks, tabs and newlines)
	 * @param newline the newline indicator, makes row++. newline automatically becomes skipper
	 */
	Tokenizer(const char* separater, const char* skipper = " \t\n", const char* newline = "\n")
		:sep(separater), skipper(skipper), newline(newline) {
			this->skipper.add(newline);
	}
	~Tokenizer() {}

public:
	/**
	 * split strings into statements. A statement is a set of string separated by seperator.
	 * this function does not skip skippers and newline
	 * e.g: make seperater be '-;', then "aaaa- bbbbxxxx d;-" is splited into "aaaa" " bbbbxxxx d;" ""
	 */
	tokenlist_type split(const char* input) const ;
	/**
	 * split string into tokens by the rules above, return the list of tokens, as well as their
	 * row & col
	 */
	tokenlist_type tokenize(const statement_type& statement) const {
		return do_tokenize(statement->c_str(), statement.row, statement.col);
	}
	tokenlist_type tokenize(const char* input) const ;
	tokenlist_type tokenize(const std::string& input) const { tokenize(input.c_str()); }
private:
	/**
	 * split statement into tokens, by the rules above, row & col starts at start_row, start_col
	 */
	tokenlist_type do_tokenize(const char* input, int start_row, int start_col) const ;
	
	const char* parseInt(const char* p, token& result) const;
	const char* parseDouble(const char* p, token& result) const;
	const char* parseQuote(const char* p, token& result) const;
	const char* parseSingleQuote(const char* p, token& result) const;
	const char* parseWord(const char* p, token& result) const ;
	const char* parseSymbol(const char* p, token& result) const;
private:
	/**
	 * the separater and skipper, force user to assign them in construction
	 * note: a better solution should be making this assigned at compile time, using mpl::string, or constexpr
	 */
	detail::CharHash sep;
	detail::CharHash skipper;
	detail::CharHash newline;
};

#endif // TOKENIZER_HPP