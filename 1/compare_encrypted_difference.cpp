/** ОПИСАНИЕ
		Эта программа определяет насколько отличаются зашифрованные тексты размером 3к символов по частотности от частотности русского языка. В файл выводятся все различия. Причем тексты шифруются не случайными ключами, а ключами с некоторым количеством перестановок букв.

	ОПИСАНИЕ РЕЗУЛЬТАТОВ
		Зачем эти данные нужны, смотри compare_texts_difference.cpp.
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

	//-------------------------------------------------------------------------
	Frequency f;
	f.load("frequency_table.txt");
	
	//-------------------------------------------------------------------------
	// Перебираем все случайные тексты, шифруем их и получаем как их частотность отличается от частотности в русском языке.
	const int textSize = 3000;
	const int keyCount = alphabetSize;
	const int textsCount = texts.size();
	const int swapCount = alphabetSize;
	const int allCount = keyCount * swapCount * textsCount;
	int count = 0;

	std::vector<std::vector<double>> encryptDifference1(alphabetSize);
	std::vector<std::vector<double>> encryptDifference2(alphabetSize);
	std::vector<std::vector<double>> encryptDifference3(alphabetSize);
	std::vector<std::vector<double>> encryptDifferenceW(alphabetSize);
	for (int i = 0; i < textsCount; ++i) {
		std::string text;
		texts.readNextText(text);
		for (int j = 0; j < swapCount; ++j) {
			for (int k = 0; k < keyCount; ++k) {
				std::string text1 = getRandomPart(text, textSize);

				if (text1.size() < textSize)
					break;

				Cipher cipher;
				cipher.generateHalfRandomKey(j+1);
				text1 = cipher.encrypt(text1);

				Frequency f1;
				f1.count(text1);
				f1.normalize();

				auto diff = f1.countDifference(f);
				encryptDifference1[j].push_back(diff.d1);
				encryptDifference2[j].push_back(diff.d2);
				encryptDifference3[j].push_back(diff.d3);
				encryptDifferenceW[j].push_back(differenceMetric(diff));

				// Выводим прогресс на экран
				count++;
				std::cout << "\r" << std::setprecision(3) << std::setw(6) << double(count * 100)/allCount << "%";
			}
		}	
	}

	//-------------------------------------------------------------------------
	std::ofstream fout("difference_encrypted_3k_text.txt");

	for (int i = 0; i < alphabetSize; ++i) {
		fout << "-------------------------------------------------------------------------------" << std::endl;
		fout << "Swap count: " << i << std::endl << std::endl;
		fout << "One letter: " << std::endl;
		fout << characterizeRandomValueByData(encryptDifference1[i]) << std::endl;
		
		fout << "Two letter: " << std::endl;
		fout << characterizeRandomValueByData(encryptDifference2[i]) << std::endl;

		fout << "Three letter: " << std::endl;
		fout << characterizeRandomValueByData(encryptDifference3[i]) << std::endl;

		fout << "DifferenceMetric difference: " << std::endl;
		fout << characterizeRandomValueByData(encryptDifferenceW[i]) << std::endl;
	}
	
	fout.close();
}