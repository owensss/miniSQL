#ifndef PARSE_EXCEPTION_H
#define PARSE_EXCEPTION_H

#include <exception>
#include "Tokenizer.hpp"
struct token;

class parse_exception : public std::exception {
public:
	parse_exception(const token& tk) throw();
	virtual ~parse_exception() throw() {}

	virtual const char* what() const {
		return "parse error";
	}

	const token& where() const;
private:
	token tk;
};

class multiline_quote_exception : public parse_exception {
public:
	multiline_quote_exception(const token& tk) throw() :parse_exception(tk) {}
	~multiline_quote_exception() {}

	const char* what() const {
		return "newline inside a quote";
	}
};

#endif // PARSE_EXCEPTION_H