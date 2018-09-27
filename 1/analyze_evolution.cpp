/** ОПИСАНИЕ
		Эта программа численно определяет насколько хорошо работает эволюция для расшифровки текстов.

	ПРИНЦИП РАБОТЫ
		Перебираются все тексты в папке для тестовых текстов. 
		Перебираются случайные их части.
		Перебираются случайные ключи.
			Эта часть зашифровывается ключом.
			Эволюция пытается расшифровать текст при помощи частотного анализа.
			Полученный ключ применяется для расшифровки текста и сравнивается эффективность расшифровки.

		Эффективность расшифровки - случайная величина, и она может меняться для разных текстов.

		Далее в файл выводится среднее значений этой случайной величины, дисперсия, минимальное и максимальное значение.

	ОПИСАНИЕ РЕЗУЛЬТАТОВ
		Чем больше к 100 среднее, тем лучше.
		Чем меньше дисперсия вкупе с предыдущим, тем лучше.
		Чем больше минимальное значение, тем лучше.
		Если максимальное значение равно 100, то хотя бы один текст расшифровался полностью правильно.
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "encrypt.h"
#include "genetic.h"

int main() {
	//-------------------------------------------------------------------------
	// Получаем внешние данные
	TextReader texts(true);

	Frequency f;
	f.load("frequency_table.txt");
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты и получаем значения случайной величины
	const int textSize = 3000;
	const int partCount = 5;
	const int keyCount = 5;
	const int textsCount = texts.size();
	const int decryptCount = partCount * keyCount * textsCount;
	int count = 0;

	std::vector<double> accuracy;
	for (int i = 0; i < textsCount; ++i) { // Цикл по текстам
		std::string text;
		texts.readNextText(text);
		for (int j = 0; j < partCount; ++j) { // Цикл по частям текстов
			std::string original = getRandomPart(text, textSize);

			if (original.size() < 3000)
				break;

			for (int k = 0; k < keyCount; ++k) { // Цикл по различным ключам
				Cipher cipher;
				cipher.generateRandomKey();
				std::string encrypted = cipher.encrypt(original);

				std::string bestKey = decryptByEvolution(encrypted, false);
				cipher.setKey(bestKey);
				std::string decrypted = cipher.decrypt(encrypted);
				accuracy.push_back(getAccuracy(original, decrypted));

				// Выводим прогресс на экран
				count++;
				std::cout << "\r" << std::setw(5) << std::setprecision(4) << double(count * 100)/decryptCount << "%";
			}
		}
	}

	//-------------------------------------------------------------------------
	// Выводим в файл характеристику полученных данных
	std::ofstream fout("evolution_efficiency.txt");
	fout << characterizeRandomValueByData(accuracy);
	fout.close();
}