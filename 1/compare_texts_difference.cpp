/** ОПИСАНИЕ
		Эта программа определяет насколько отличаются случайные тексты размером 3к символов по частотности от частотности русского языка. В файл выводятся все различия.

	ОПИСАНИЕ РЕЗУЛЬТАТОВ
		Эти данные нужны, чтобы затем сравнить их между данными, когда тексты зашифрованы ключом с m=1,n перестановками, чтобы получить потенциальную теоретическую точность расшифровки текстов длиной 3к.

		Если по какому-то параметру, либо комбинации параметров получается, что тексты в среднем различаются на меньший процент, чем зашифрованный ключом с 1 перестановкой, то это означает, что в теории возможно большинство текстов расшифровать полностью.
 */

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>

#include "encrypt.h"

int main() {
	//-------------------------------------------------------------------------
	// Получаем все тексты
	TextReader texts(true);

	Frequency f;
	f.load("frequency_table.txt");
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты и получаем как их частотность отличается от частотности в русском языке.
	const int textSize = 3000;
	const int partCount = 5;
	const int textsCount = texts.size();
	const int allCount = partCount * textsCount;
	int count = 0;

	std::vector<double> difference1;
	std::vector<double> difference2;
	std::vector<double> difference3;
	std::vector<double> differenceW;
	for (int i = 0; i < textsCount; ++i) {
		std::string text;
		texts.readNextText(text);
		for (int j = 0; j < partCount; ++j) {
			std::string text1 = getRandomPart(text, textSize);

			if (text1.size() < textSize)
				break;

			Frequency f1;
			f1.count(text1);
			f1.normalize();

			auto diff = f1.countDifference(f);

			difference1.push_back(diff.d1);
			difference2.push_back(diff.d2);	
			difference3.push_back(diff.d3);
			differenceW.push_back(differenceMetric(diff));

			// Выводим прогресс на экран
			count++;
			std::cout << "\r" << std::setprecision(4) << std::setw(5)  << double(count * 100)/allCount << "%";
		}
	}

	//-------------------------------------------------------------------------
	// Выводим в файл полученные данные
	std::ofstream fout("difference_3k_text.txt");

	fout << "One letter: " << std::endl;
	fout << characterizeRandomValueByData(difference1) << std::endl;
	
	fout << "Two letter: " << std::endl;
	fout << characterizeRandomValueByData(difference2) << std::endl;

	fout << "Three letter: " << std::endl;
	fout << characterizeRandomValueByData(difference3) << std::endl;

	fout << "DifferenceMetric difference: " << std::endl;
	fout << characterizeRandomValueByData(differenceW) << std::endl;

	fout.close();
}