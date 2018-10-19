#pragma once
#include <string>
#include "Lexeme.h"

enum class DescriptorType { tableSymbol, serviceWords, identifiers, constants };

struct Descriptor
{	
	DescriptorType type;
	std::string value;
	
	Descriptor() :value{ "" } {}
	Descriptor(DescriptorType t, std::string v) :type{ t }, value{ v } {}
	Descriptor(Lexeme lexeme);
	friend std::ostream& operator << (std::ostream& out, const DescriptorType& t);
};

Descriptor::Descriptor(Lexeme lexeme)
{
	value = lexeme.value;
	switch (lexeme.type) {
	case Lexeme::TypeLexeme::tableSymbol: type = DescriptorType::tableSymbol; break;
	case Lexeme::TypeLexeme::serviceWords: type = DescriptorType::serviceWords; break;
	case Lexeme::TypeLexeme::constants: type = DescriptorType::constants; break;
	case Lexeme::TypeLexeme::identifiers: type = DescriptorType::identifiers; break;
	}
}

std::ostream& operator << (std::ostream& out, const DescriptorType& t)
{
	switch (t){
		case DescriptorType::tableSymbol: return (out << "tableSymbol");
		case DescriptorType::serviceWords: return (out << "serviceWords");
		case DescriptorType::constants: return (out << "constants");
		case DescriptorType::identifiers: return (out << "identifiers");
	}
	return (out);
}