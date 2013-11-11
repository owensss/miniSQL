#ifndef CHAR_HASH_HPP
#define CHAR_HASH_HPP

#include <array>

namespace detail {

class CharHash {
public:
	enum {CHAR_SIZE = 255};
	typedef std::array<bool, CHAR_SIZE> char_hash_type;
public:
	CharHash( const char* str ) {
		clear();
		add(str);
	}

	void add(const char* str) {
		if (str == nullptr) return;
		for (const char* p = str; *p != '\0'; ++p)
			add(*p);
	}

	void add(char c) {
		hash[c] = true;
	}

	void remove(const char* str) {
		if (str == nullptr) return;
		for (const char* p = str; *p != '\0'; ++p)
			remove(*p);
	}

	void remove(char c) {
		hash[c] = false;
	}

	void clear() { hash.fill(false);}

	bool operator [] (const char c) {
		return hash[c];
	}

	bool operator [] (const char c) const {
		return hash[c];
	}
private:
	char_hash_type hash;
};

} // detail
#endif // CHAR_HASH