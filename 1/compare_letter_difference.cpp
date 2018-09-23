#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "encrypt.h"

int main() {
	//-------------------------------------------------------------------------
	// Получаем все тексты
	std::vector<std::string> texts = readTexts();

	//-------------------------------------------------------------------------
	Frequency f;
	f.load("frequency_table.txt");
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты и получаем из разницу между их частями длиной 3000 символов
	std::vector<std::vector<double>> letterData1(alphabetSize); 
	std::vector<std::vector<double>> letterData2(alphabetSize); 
	for (int i = 0; i < texts.size(); ++i) {
		for (int j = 0; j < 5; ++j) {
			std::string text1 = getRandomPart(texts[i], 3000);

			if (text1.size() < 3000)
				break;

			Frequency f1;
			f1.count(text1);
			f1.normalize();

			for (int i = 0; i < alphabetSize; ++i) {
				auto diff = f1.countDifference(f, rus_a + i);
				letterData1[i].push_back(diff.first);
				letterData2[i].push_back(diff.second);
			}	
		}
	}

	//-------------------------------------------------------------------------
	// Различие между случайными текстами есть случайная величина. Считаем ее минимальное, максимальное, среднее и среднеквадратичное значение
	std::ofstream fout("difference_letters.txt");
	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	fout << std::fixed;

	for (int i = 0; i < alphabetSize; ++i) {
		auto charact1 = characterizeRandomValueByData(letterData1[i]);
		auto charact2 = characterizeRandomValueByData(letterData2[i]);
		fout << char(rus_a + i) << "\t" << charact1.expected * 100 << "\t" << charact2.expected * 100 << std::endl;
	}

	fout.close();
}