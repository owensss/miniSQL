#pragma once
#include <exception>

//TODO
class AlreadyExists :
	public std::exception
{
public:
	AlreadyExists(const char* msg){
		this->msg = new char[strlen(msg)+1];
		strcpy(this->msg, msg);
	}

	AlreadyExists(void):msg(nullptr){}
	~AlreadyExists(){
		delete [] msg;
	}
	const char* what() const throw(){
		return strcat(msg, " already exists, cannot be added again");
	};
private:
	char* msg;
};

class CannotInsert :
	public std::exception
{
public:
	CannotInsert(const char* msg){
		this->msg = new char[strlen(msg)+1];
		strcpy(this->msg, msg);
	}

	CannotInsert(void):msg(nullptr){}
	~CannotInsert(){
		delete [] msg;
	}
	const char* what() const throw(){
		return strcat("Cannot open the file", msg);
	};
private:
	char* msg;
};