#pragma once
#include <string>

struct Error
{
	std::string errorMes;
	Error() :errorMes{ "Error : " } {}
	Error(std::string message) : errorMes{ "Erroe : " + message + " \n" } {}
};

struct  Lexeme
{
	enum class TypeLexeme { tableSymbol, serviceWords, identifiers, constants, error };
	TypeLexeme type;
	std::string value;

	Lexeme() :value{ "" } {}
	Lexeme(TypeLexeme t, std::string v) :type{ t }, value{ v } {}
	bool Empty()//если тип не определен, то false
	{
		if (this->type != TypeLexeme::error && this->type != TypeLexeme::constants && this->type != TypeLexeme::identifiers && this->type != TypeLexeme::serviceWords && this->type != TypeLexeme::tableSymbol)
			return false;		
		return	true;
	}
};
