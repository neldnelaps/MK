#pragma once
#include "Scaner.h"
#include "Lexeme.h"
#include "Descriptor.h"
#include <iostream>

class LexicalAnalysis
{
	Scaner scaner;
	std::list<std::string> identifiersTable;
	std::list<std::string> constantsTable;
	bool hasDefinition = false; // имеет  определение
	std::list<Descriptor> lexicalConvolution;	

public:
	LexicalAnalysis() : identifiersTable{ std::list<std::string>() }, constantsTable{ std::list<std::string>() }, lexicalConvolution{ std::list<Descriptor>() } {}
	void LoadProgramm(std::string path);
	bool Analyze();
	void DeleteDescription();
	void PrintLexicalConvolution();
	std::list<std::string> GetConvolution();
};

void LexicalAnalysis::LoadProgramm(std::string path)
{
	scaner.LoadProgramm(path);
}

bool LexicalAnalysis::Analyze()
{
	Lexeme lexeme = scaner.NextLexeme();

	while (lexeme.Empty()) { // если лексемы нет, выход
		if (lexeme.type == Lexeme::TypeLexeme::error) {
			throw Error("Lexical error!");
			lexicalConvolution.clear();
			return false;
		}
		if (lexeme.type == Lexeme::TypeLexeme::tableSymbol || lexeme.type == Lexeme::TypeLexeme::serviceWords) {
			if (lexeme.value == "int")
				hasDefinition = true;

			if (lexeme.value == ";")
				hasDefinition = false;

			if (lexeme.value != " ")
				lexicalConvolution.push_back(Descriptor(lexeme));
		}
		else if (lexeme.type == Lexeme::TypeLexeme::identifiers) {
			if (scaner.Contains(identifiersTable, lexeme.value) == false) {
				if (hasDefinition == true) {
					identifiersTable.push_back(lexeme.value);
					lexicalConvolution.push_back(Descriptor(lexeme));
				}
				else {
					throw Error("There is no specific entry.");
					lexicalConvolution.clear();
					return false;
				}
			}
			else {
				if (hasDefinition == true) {
					throw Error("Re-define the name.");
					lexicalConvolution.clear();
					return false;
				}
				else
					lexicalConvolution.push_back(Descriptor(DescriptorType::identifiers, lexeme.value));
			}
		}
		else if (lexeme.type == Lexeme::TypeLexeme::constants) {
			if (!scaner.Contains(constantsTable, lexeme.value))
				constantsTable.push_back(lexeme.value);
			lexicalConvolution.push_back(Descriptor(lexeme));
		}
		lexeme = scaner.NextLexeme();
	}
	return true;
}

void LexicalAnalysis::DeleteDescription() //Описания из лексической свертки удаляем.
{
	for (std::list<Descriptor>::iterator it = lexicalConvolution.begin(); it != lexicalConvolution.end();) {
		if (it->value == "int") {
			while (it->value != ";") {
				it = lexicalConvolution.erase(it);
			}
			it = lexicalConvolution.erase(it);
		}
		else
			++it;
	}
}

void LexicalAnalysis::PrintLexicalConvolution() //Вывод лексической свертки 
{
	for (std::list<Descriptor>::iterator k = lexicalConvolution.begin(); k != lexicalConvolution.end(); ++k)
		std::cout << k->type << ", " << '\t' << k->value << std::endl;
}

std::list<std::string> LexicalAnalysis::GetConvolution()
{
	std::list<std::string> convolution = std::list<std::string>();
	for (auto k : lexicalConvolution) {
		if (k.type == DescriptorType::constants)
			convolution.push_back("constants");
		else if (k.type == DescriptorType::identifiers)
			convolution.push_back("identifiers");
		else
			convolution.push_back(k.value);
	}
	return convolution;
}