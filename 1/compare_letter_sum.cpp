#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>

#include "encrypt.h"

int main() {
	//-------------------------------------------------------------------------
	// Получаем все тексты
	std::vector<std::string> texts = readTexts();
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты и получаем из разницу между их частями длиной 3000 символов
	std::vector<std::vector<double>> letterDifference(alphabetSize); 
	for (int i = 0; i < texts.size(); ++i) {
		for (int j = 0; j < 5; ++j) {
			std::string text1 = getRandomPart(texts[i], 3000);

			Frequency f1;
			f1.count(text1);
			f1.normalize();

			for (int k = 0; k < f1.f1.size(); ++k) {
				letterDifference[k].push_back(0);
				auto& sum = letterDifference[k].back();
				for (int m = 0; m < f1.f1.size(); ++m) {
					if (i != j)
						sum += metric(f1.f1[k], f1.f1[m]);
				}
				sum /= f1.f1.size() - 1;
			}
		}
	}

	//-------------------------------------------------------------------------
	// Различие между случайными текстами есть случайная величина. Считаем ее минимальное, максимальное, среднее и среднеквадратичное значение
	std::ofstream fout("difference_letters_sum.txt");
	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	fout << std::fixed;

	for (int i = 0; i < letterDifference.size(); ++i) {
		auto charact = characterizeRandomValueByData(letterDifference[i]);
		fout << char(rus_a + i) << "\t" << charact.expected * 100 << std::endl;
	}

	fout.close();
}