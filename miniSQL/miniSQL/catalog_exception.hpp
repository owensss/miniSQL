# pragma once
#include <exception>
#include <string>

class catalog_exception : public std::exception {
public:
	catalog_exception(const std::string& name) throw() : name(name){}
	virtual ~catalog_exception() throw() {}

	virtual const char* what() const = 0;

	const std::string& getName() { return name; }
private:
	std::string name; //name of relation or index
};

class duplicate : public catalog_exception {
public:
	duplicate(const std::string& name) : catalog_exception(name){}
	~duplicate() throw() {}

	const char* what(){ return (getName() + "duplicate").c_str() ;}
};

attri_duplicate
prima
not_exists
