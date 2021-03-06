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

class unexpected_token_exception : public parse_exception {
public:
	unexpected_token_exception(const token& tk, const char* expect) throw() 
		:parse_exception(tk), expect_(new char[255]) {
			strncpy_s(expect_, 254, expect, 254);
	}
	~unexpected_token_exception() throw() {}
	
	const char* what() const {
		return "unexpected token";
	}
	
	const char* expect() const {
		return expect_;
	}
private:
	char* expect_;
};

class unexpected_end_of_input_exception : public parse_exception {
public:
	unexpected_end_of_input_exception(const token& tk, const char* expect) 
		throw() :parse_exception(tk) , expect_(new char[255]) {
			strncpy_s(expect_, 254, expect, 254);
	}
	~unexpected_end_of_input_exception() {}

	const char* what() const {
		return "unexpectd end of input";
	}

	const char* expect() const {
		return expect_;
	}

private:
	char* expect_;
};

#endif // PARSE_EXCEPTION_H