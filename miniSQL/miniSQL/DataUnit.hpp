#pragma once

union DataUnit{
	DataUnit(char* str):str(str){};
	DataUnit(int integer):integer(integer){};
	DataUnit(float fl):fl(fl){};
	char* str; //ptr to a string end with '\0'
	int	integer;
	float fl;
};