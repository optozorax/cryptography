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

	Frequency f;
	f.load("frequency_table.txt");
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты и получаем из разницу между их частями длиной 3000 символов
	std::vector<double> difference1;
	std::vector<double> difference2;
	for (int i = 0; i < texts.size(); ++i) {
		for (int j = 0; j < 5; ++j) {
			std::string text1 = getRandomPart(texts[i], 3000);

			if (text1.size() < 3000)
				break;

			Frequency f1;
			f1.count(text1);
			f1.normalize();

			auto diff = f1.countDifference(f);
			difference1.push_back(diff.first);
			difference2.push_back(diff.second);	
		}
	}

	//-------------------------------------------------------------------------
	// Различие между случайными текстами есть случайная величина. Считаем ее минимальное, максимальное, среднее и среднеквадратичное значение
	auto charact1 = characterizeRandomValueByData(difference1);
	auto charact2 = characterizeRandomValueByData(difference2);

	//-------------------------------------------------------------------------
	// Выводим в файл полученные данные
	std::ofstream fout("difference_texts.txt");
	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	fout << std::fixed;
	fout << "min1: " << charact1.min * 100 << std::endl;
	fout << "max1: " << charact1.max * 100 << std::endl;
	fout << "expected1: " << charact1.expected * 100 << std::endl;
	fout << "dispersion1: " << charact1.dispersion * 100 << std::endl;

	fout << std::endl;

	fout << "min2: " << charact2.min * 100 << std::endl;
	fout << "max2: " << charact2.max * 100 << std::endl;
	fout << "expected2: " << charact2.expected * 100 << std::endl;
	fout << "dispersion2: " << charact2.dispersion * 100 << std::endl;

	fout.close();
}