#pragma once
#include <list>
#include <string>
#include <fstream>
#include "Lexeme.h"

const std::list<std::string> tableSymbol = { "+", "-", "/", "*", ";", ",", "{", "}", "(", ")", ">", ">=", "<", "<=", "=", "==", "!=", "&", "|", "!", " " };
const std::list<std::string> serviceWords = { "void", "main", "int", "if", "else", "while" };

class Scaner
{	
	std::string programm;
	int currentIndex;
public:
	Scaner() :programm{ "" }, currentIndex{ 0 } {};
	bool Contains(std::list<std::string> rows, std::string str);// содержитс€ ли элемент str  в списке
	void LoadProgramm(std::string fileName);
	Lexeme NextLexeme();	// выделение лексемы
	std::list<std::string> GetAcceptableCharacters(){ return tableSymbol; }
	std::list<std::string> GetServiceWords(){ return serviceWords; }
};

bool Scaner::Contains(std::list<std::string> rows, std::string str) 
{
	if (std::find(rows.begin(), rows.end(), str) != rows.end())
		return true;
	return false;
}

void Scaner::LoadProgramm(std::string fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) // если файл не открыт
		throw Error("Error!!! File is not open.");
	else {
		char ch = { ' ' }, prev = { ' ' };
		while ((ch = file.get()) != EOF) 
			if (ch != '\n' && ch && '\r' && ch != '\t'){
				if (ch != ' ' || prev != ' '){
					programm += ch;
					prev = ch;
				}
			}		
	}
}

Lexeme Scaner::NextLexeme() 
{
	Lexeme lexeme;
	std::string buffer = "";
	std::string str = "";
	str = programm[currentIndex];
	if (currentIndex < programm.length() && Contains(tableSymbol, str) == true) { // если str состоит в  таблице tableSymbol
		if (currentIndex < programm.length()) {
			str += programm[++currentIndex];
			if (Contains(tableSymbol, str) == false) {
				str.erase(str.end() - 1);
				--currentIndex;
			}
		}
		++currentIndex;
		lexeme = Lexeme(Lexeme::TypeLexeme::tableSymbol, str);
		return lexeme;
	}
	while (currentIndex < programm.length() && Contains(tableSymbol, str) == false) {// если str не состоит в  таблице tableSymbol
		buffer += programm[currentIndex];
		++currentIndex;
		str = programm[currentIndex];
	}

	if (buffer.length() > 0) {
		Lexeme::TypeLexeme tmpType = Lexeme::TypeLexeme::error;

		if (Contains(serviceWords, buffer) == true)
			tmpType = Lexeme::TypeLexeme::serviceWords;
		else if (isdigit(buffer[0])) // если число
		{
			for (int i = 1; i < buffer.length(); i++)
			{
				if (!isdigit(buffer[i]))
					throw Error("invalid number");
			}
			tmpType = Lexeme::TypeLexeme::constants;
		}
		else if (isalpha(buffer[0]))	//если переменна€		
			tmpType = Lexeme::TypeLexeme::identifiers;

		lexeme = Lexeme(tmpType, buffer);
	}
	return lexeme;
}