#include "stdafx.h";
#include <fstream>
#include <iostream>
#include <sstream>
#include "LexicalAnalysis.h"
#include "Scaner.h"
#include "Recognizer.h"


int main()
{
	try {
		LexicalAnalysis  analyzer;
		analyzer.LoadProgramm("test4.txt");
		if (analyzer.Analyze() == true) {
			analyzer.DeleteDescription(); // удаление описания из лексической свертки	
			analyzer.PrintLexicalConvolution(); // вывод лексической свертки	
			std::list<std::string> st = analyzer.GetConvolution();
			Recognizer recognizer =  Recognizer("Program", st);
			std::cout << std::endl;
			recognizer.Parse();
		}
		return 0;
	}
	catch (Error& ex)
	{
		std::cerr << ex.errorMes;
	}
	catch (...)
	{
		std::cerr << "Error";
	}
}
