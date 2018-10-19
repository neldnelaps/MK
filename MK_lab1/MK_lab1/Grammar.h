#pragma once
#include <list>
#include <string>

struct Grammar
{
	std::string left;
	std::list<std::string> right; 

	Grammar() :left{ "" }{}
	Grammar(std::string l, std::list<std::string> v) :left{ l }, right{ std::list<std::string>() } {}
};
