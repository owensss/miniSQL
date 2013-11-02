#pragma once
#include <exception>
class NoSubstitution :
	public std::exception
{
public:
	const char* what() const throw(){
		return "No substitution in buffer!\nAll blockes are pinned";
	};
};

class CannotOpenFile :
	public std::exception
{
public:
	CannotOpenFile(const char* msg){
		this->msg = new char[strlen(msg)+1];
		strcpy(this->msg, msg);
	}

	CannotOpenFile(void):msg(nullptr){}
	~CannotOpenFile(){
		delete [] msg;
	}
	const char* what() const throw(){
		return strcat("Cannot open the file", msg);
	};
private:
	char* msg;
};