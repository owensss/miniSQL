# pragma once
#include <exception>
#include <string>

class frequent_exception : public std::exception {
public:
	frequent_exception(const std::string& name) throw() : name(name){}
	virtual ~frequent_exception() throw() {}

	virtual const char* what() const = 0;

	const std::string& getName() const{ return name; }
private:
	std::string name; //name of relation or index
};

class duplicate : public frequent_exception {
public:
	duplicate(const std::string& name) throw() : frequent_exception(name){}
	~duplicate() throw() {}

	const char* what() const{ return (getName() + " duplicates.").c_str() ;}
};

class not_exists : public frequent_exception {
public:
	not_exists(const std::string& name) throw() : frequent_exception(name){}
	~not_exists() throw() {}

	const char* what() const{ return (getName() + " duplicates.").c_str() ;}
};

class not_match : public frequent_exception {
public:
	not_match(const std::string& lhs, const std::string& rhs) throw() : frequent_exception(lhs), rhs(rhs){}
	~not_match() throw() {}

	const char* what() const{ return (getName() + " not match with " + rhs).c_str() ;}
private:
	std::string rhs;
};