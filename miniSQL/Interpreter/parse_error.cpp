#include "parse_exception.h"
#include "Tokenizer.hpp"

parse_exception::parse_exception(const token& tk) throw()
	:tk(tk) {
}

const token& parse_exception::where() const {
	return tk;
}