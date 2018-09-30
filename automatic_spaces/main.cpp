#include <iostream>
#include "evolution.h"

int main() {
	std::string text = "необходимо собрать статистику по предложению и да что тому итого потому";

	std::cout << Evaluator().calcEfficiency(text) << std::endl;

	std::string textNoSpaces = toRussianLetters(text);
	Evolution ev(40, 13, 96, textNoSpaces, Evaluator());
	std::string text2 = ev.evolveToResult();

	writeToFile("spaced.txt", text + "\n" + text2);

	double result = getAccuracySpaces(text, text2) * 100.0;
	std::cout << result << std::endl;

	system("pause");
}