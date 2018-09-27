#include <iostream>
#include <fstream>
#include <algorithm>
#include "encrypt.h"
#include "genetic.h"

int main() {
	//-------------------------------------------------------------------------
	// Получаем все тексты
	TextReader texts(true);

	Frequency f;
	f.load("frequency_table.txt");
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты и получаем из разницу между их частями длиной 3000 символов
	const int partCount = 5;
	const int keyCount = 5;
	int textsCount = std::min(texts.size(), 10);

	int decryptCount = partCount * keyCount * textsCount;
	int count = 0;

	std::vector<double> accuracy;
	for (int i = 0; i < textsCount; ++i) { // Цикл по текстам
		std::string text;
		texts.readNextText(text);
		for (int j = 0; j < partCount; ++j) { // Цикл по частям текстов
			std::string original = getRandomPart(text, 3000);

			for (int k = 0; k < keyCount; ++k) { // Цикл по различным ключам
				Cipher cipher;
				cipher.generateRandomKey();

				std::string encrypted = cipher.encrypt(original);

				std::string bestKey = decryptByEvolution(encrypted, false);
				cipher.setKey(bestKey);

				std::string decrypted = cipher.decrypt(encrypted);

				accuracy.push_back(getAccuracy(original, decrypted));

				count++;
				std::cout 
					<< "\r" << double(count * 100)/decryptCount << "%         ";
			}
		}
	}

	//-------------------------------------------------------------------------
	// Различие между случайными текстами есть случайная величина. Считаем ее минимальное, максимальное, среднее и среднеквадратичное значение
	auto charact = characterizeRandomValueByData(accuracy);

	//-------------------------------------------------------------------------
	// Выводим в файл полученные данные
	std::ofstream fout("evolution_analyze.txt");
	fout.precision(4);
	fout << std::fixed;
	fout << "min: " << charact.min * 100 << std::endl;
	fout << "max: " << charact.max * 100 << std::endl;
	fout << "expected: " << charact.expected * 100 << std::endl;
	fout << "dispersion: " << charact.dispersion * 100 << std::endl;

	fout.close();
}