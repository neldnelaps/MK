#pragma once
#include <string>
#include <list>
#include <stack>
#include "Grammar.h"
#include <algorithm> 

enum class  State { normal, back, finish };

struct infNonterminal
{
	std::string nonterminal;
	int alt_count;
	int first_alt;
	infNonterminal() :nonterminal{ "" }, alt_count{ 0 }, first_alt{ 0 } {}	
	infNonterminal(std::string n, int a, int f) :nonterminal{ n }, alt_count{ a }, first_alt{ f } {}
	
};
struct symbolType
{
	char is_terminal;
	std::string symbol;
	int alt_number;
	int alt_count;
	void Set(char is_t, std::string sym, int n, int c)
	{
		is_terminal = is_t;
		symbol = sym;
		alt_number = n;
		alt_count = c;
	}
};
struct Symbol
{
	char is_terminal;
	std::string symbol;

	void Set(char is_t, std::string sym)
	{
		is_terminal = is_t;
		symbol = sym;
	}
};

class Recognizer
{	
	std::list<Grammar> grammar; // грамматика	
	std::list<std::string> terminals; //терминалы
	std::list<std::string> left_part; // левая часть грамматики
	std::list<std::string> rigth_part; // правая часть грамматики 
	std::list<std::string> strConvolution; // свертка
	std::list<infNonterminal> inf_nonterminal; // не терминалы
	
	std::stack<symbolType> substitution_history;
	symbolType history_symbol; // история символов
	std::stack<Symbol> current_sequence; // текущая последовательность
	Symbol current_symbol; // текущий символ

	std::stack<int> result; // результат
	State state; // состояние: normal, back, finish
	std::string S = ""; // programm 

public:
	Recognizer()
	{
		grammar = std::list<Grammar>();
		terminals = std::list<std::string>();
		left_part = std::list<std::string>();
		rigth_part = std::list<std::string>();
		strConvolution = std::list<std::string>();
		inf_nonterminal = std::list<infNonterminal>();
		history_symbol = symbolType();
		substitution_history = std::stack<symbolType>();
		current_sequence = std::stack<Symbol>();
		current_symbol = Symbol();
		result = std::stack<int>();
	}
	int findTerminal(std::string s); // поиск терминала
	int findNonterminal(std::string s); // поиск не терминала
	std::string findString(int str_index); 
	std::list<Grammar> LoadGrammar(std::string fileName); // функция для формирования грамматики по файлу
	Recognizer(std::string beginSymbol, std::list<std::string> text);
	int GetAlternativeCount(std::string name); // Получить альтернативный счет
	void PrintGrammar(); // вывод грамматики
	void Parse();	
	std::list<std::string> GetRightParts(); // получить правую часть грамматики
	std::list<std::string> GetLeftParts(); // получить левую часть грамматики
	void popRigthPart(int rule_number);
	void pushRigthPart(int rule_number);
	void pushLeftPart(int nRule);

	void FirstStep();
	void SecondStep();
	void ThirdStep();
	void FifthStep();
	void SixthStepA();
	void SixthStepB();	
};

int Recognizer::findTerminal(std::string s)
{
	int i = 0;
	for (auto k : terminals) {
		if (k == s)
			return i;
		i++;
	}
	return -1;
}

int Recognizer::findNonterminal(std::string s)
{
	int i = 0;
	for (auto f : inf_nonterminal) {
		if (f.nonterminal == s)
			return i;
		i++;
	}
	return -1;
}

std::string Recognizer::findString(int str_index)
{
	int i = 0;
	std::string st;
	for (auto k : strConvolution) {
		if (i == str_index) {
			st = k;
			return st;
		}
		i++;
	}
	return st;
}

std::list<Grammar> Recognizer::LoadGrammar(std::string fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) // если файл не открыт
		throw Error("Error!!! File is not open.");
	else {
		std::list<Grammar> grammar;
		std::string line;
		while (!file.eof()) {
			getline(file, line);
			Grammar gr;
			std::stringstream str_file{ line };
			std::string str;

			str_file >> str;
			gr.left = str;
			str_file >> str; // пропуск ->

			std::string buffer{ "" };
			for (; str_file >> str;) {
				if (str != "$")
					buffer += str + ' ';
				else
				{
					if (buffer[buffer.size() - 1] == ' ')
						buffer.erase(buffer.size() - 1, 1);
					gr.right.push_back(buffer);
					buffer = "";
				}
			}
			if (buffer[buffer.size() - 1] == ' ')
				buffer.erase(buffer.size() - 1, 1);
			gr.right.push_back(buffer);
			grammar.push_back(gr);
		}
		return grammar;
	}
}

Recognizer::Recognizer(std::string beginSymbol, std::list<std::string> text)
{
	S = beginSymbol;
	strConvolution = text;

	grammar = LoadGrammar("Grammar.txt");
	Scaner scaner = Scaner();
	terminals = scaner.GetAcceptableCharacters(); // формирование списка терминалов
	auto it = terminals.end();
	terminals.splice(it, scaner.GetServiceWords());
	terminals.push_back("constants");
	terminals.push_back("identifiers");

	left_part = GetLeftParts();
	rigth_part = GetRightParts();

	std::list<std::string> uniqLeftPart = left_part;

	uniqLeftPart.unique(); // левая часть грамматики без повторений

	int index = 0;
	for (auto f : uniqLeftPart) { // заполнение inf_nonterminal нетерминалами 
		int alternativeCount = GetAlternativeCount(f);
		inf_nonterminal.push_back(infNonterminal(f, alternativeCount, index));
		index += alternativeCount;
	}
}

int Recognizer::GetAlternativeCount(std::string name)
{
	int count = 0;
	for (auto list : grammar)
		if (list.left == name)
			count += list.right.size();
	return count;
}

void Recognizer::PrintGrammar() 
{
	int i = 1;	
	for (auto f : grammar) {
		std::cout << f.left << " -> " << std::endl;
		for (auto gg : f.right){
			std::cout << i << ": " << "\t\t" << gg << std::endl;
			i++;
		}
	}
}

void Recognizer::Parse()
{	
	int str_index = 0;
	bool stop = false;

	state = State::normal;
	current_symbol.Set(0, S);
	current_sequence.push(current_symbol);
	PrintGrammar(); // вывод грамматики

	while (!stop){
		switch (state) {
		case State::finish: {
			std::cout << "Output: " << result.top();
			result.pop();
			while (result.size() > 0) {
				std::cout << " -> " << result.top();
				result.pop();
			}
			std::cout << std::endl;
			stop = true;
			break;
		}
		case State::normal: {
			int i = 0;
			std::string a = findString(str_index);
			current_symbol = current_sequence.top();
			if (findNonterminal(current_symbol.symbol) != -1) // first step
				FirstStep();
			else if (current_symbol.symbol == a) { //порожденный символ равен входному
				SecondStep(); // Second Step
				++str_index;
				if (str_index == strConvolution.size()) {
					if (current_sequence.size() == 0) {
						ThirdStep(); //  Third Step - Успешное завершение
						state = State::finish;
					}
					else
						state = State::back;
				}
				else
					if (current_sequence.size() == 0)
						state = State::back;			
			}										
			else					
				state = State::back;																	
			break;	
		}
		case State::back: {
			history_symbol = substitution_history.top();
			if (findTerminal(history_symbol.symbol) != -1) {
				FifthStep(); // Fifth Step - возврат по терминалу
				--str_index; // индекс входной цепочки возвращается на один символ назад
			}
			else {
				if (history_symbol.alt_number < history_symbol.alt_count) {
					SixthStepA(); // Sixth Step A -Возврат по нетерминалу (испытание новой альтернативы)
					state = State::normal;
				}
				else {
					if (history_symbol.symbol == S && str_index == 0) {				
						throw Error("Error!!! Stop!");					
						stop = true;
					}
					else // у нетерминала кончились альтернативы
						SixthStepB(); // Sixth Step B
				}
			}
			break;
		}
		}
	}
}

std::list<std::string> Recognizer::GetRightParts()
{
	std::list<std::string> rightParts = std::list<std::string>();
	for (auto list : grammar)
		for (auto f : list.right)
			rightParts.push_back(f);
	return rightParts;
}

std::list<std::string> Recognizer::GetLeftParts()
{
	std::list<std::string> leftParts = std::list<std::string>();
	for (auto list : grammar)
		for (int i = 0; i < list.right.size(); i++)
			leftParts.push_back(list.left);
	return leftParts;
}

void Recognizer::popRigthPart(int rule_number) //pop from current_sequence
{
	std::list<std::string> rules;
	int i = 0;
	for (auto k : rigth_part) {
		if (i == rule_number) {
			std::stringstream str_file{ k };
			std::string str;
			for (; str_file >> str;)
				rules.push_back(str);
			rules.reverse();
			for (auto k : rules)
				current_sequence.pop();
			return;
		}
		i++;
	}
}

void Recognizer::pushRigthPart(int rule_number)
{
	std::list<std::string> rules;
	int i = 0;
	for (auto k : rigth_part) {
		if (i == rule_number) {
			std::stringstream str_file{ k };
			std::string str;
			for (; str_file >> str;) {
				rules.push_back(str);
			}
			rules.reverse();
			for (auto k : rules)
			{
				current_symbol.symbol = k;
				if (findNonterminal(k) != -1)
					current_symbol.is_terminal = 0;
				else
					current_symbol.is_terminal = 1;
				current_sequence.push(current_symbol);
			}
			return;
		}
		i++;
	}
}

void Recognizer::pushLeftPart(int nRule)
{
	std::list<std::string> rules;
	int i = 0;
	for (auto k : left_part) {
		if (i == nRule) {
			current_symbol.symbol = k;
			if (findNonterminal(k) != -1)
				current_symbol.is_terminal = 0;
			else
				current_symbol.is_terminal = 1;
			current_sequence.push(current_symbol);
			return;
		}
		i++;
	}
}

void Recognizer::FirstStep() // в L1 формируется индекс альтернативы  L2 правая часть правила
{
	int nonterminal_index = findNonterminal(current_symbol.symbol);
	int i = 0;
	for (auto f : inf_nonterminal) {
		if (i == nonterminal_index) {
			history_symbol.Set(0, current_symbol.symbol, 1, f.alt_count);
			substitution_history.push(history_symbol);
			current_sequence.pop();
			int rule_number = f.first_alt + history_symbol.alt_number - 1;
			pushRigthPart(rule_number);
			return;
		}
		i++;
	}
}

void Recognizer::SecondStep() // Терм.символ переносится из L2 в L1
{
	current_sequence.pop();
	history_symbol.Set(1, current_symbol.symbol, 0, 0);
	substitution_history.push(history_symbol);
}

void Recognizer::ThirdStep()
{
	while (substitution_history.size() != 0){
		history_symbol = substitution_history.top();
		substitution_history.pop();
		int nonterminal_index = findNonterminal(history_symbol.symbol);
		if (nonterminal_index != -1){
			int i = 0;
			for (auto f : inf_nonterminal) {
				if (i == nonterminal_index) {
					result.push(f.first_alt + history_symbol.alt_number);
					break;
				}
				i++;
			}
		}
	}
}

void Recognizer::FifthStep()
{
	substitution_history.pop();
	current_symbol.symbol = history_symbol.symbol;
	current_symbol.is_terminal = 1;
	current_sequence.push(current_symbol);
}

void Recognizer::SixthStepA()
{
	substitution_history.pop();
	++history_symbol.alt_number;
	substitution_history.push(history_symbol);
	int nonterminal_index = findNonterminal(history_symbol.symbol);
	int i = 0;
	for (auto f : inf_nonterminal) {
		if (i == nonterminal_index) {
			int rule_number = f.first_alt + history_symbol.alt_number - 1;
			popRigthPart(rule_number - 1);
			pushRigthPart(rule_number);
			return;
		}
		i++;
	}
}

void Recognizer::SixthStepB()
{
	int nonterminal_index = findNonterminal(history_symbol.symbol);
	int i = 0;
	for (auto f : inf_nonterminal) {
		if (i == nonterminal_index) {
			int rule_number = f.first_alt + history_symbol.alt_number - 1;
			popRigthPart(rule_number);
			pushLeftPart(rule_number);
			substitution_history.pop();
			return;
		}
		i++;
	}
}