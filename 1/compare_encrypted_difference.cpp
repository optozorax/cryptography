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
	std::vector<std::vector<double>> encryptDifference1(alphabetSize);
	std::vector<std::vector<double>> encryptDifference2(alphabetSize);
	for (int i = 0; i < alphabetSize; ++i) {
		for (int j = 0; j < alphabetSize; ++j) {
			Cipher cipher;
			cipher.generateHalfRandomKey(i);
			for (int k = 0; k < texts.size(); ++k) {
				std::string text1 = getRandomPart(texts[k], 3000);

				if (text1.size() < 3000)
					break;

				text1 = cipher.encrypt(text1);

				Frequency f1;
				f1.count(text1);
				f1.normalize();

				auto diff = f1.countDifference(f);
				encryptDifference1[i].push_back(diff.first);
				encryptDifference2[i].push_back(diff.second);
			}
		}
	}

	//-------------------------------------------------------------------------
	std::ofstream fout("difference_encrypted.txt");
	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	fout << std::fixed;
	
	for (int i = 0; i < encryptDifference1.size(); ++i) {
		auto charact1 = characterizeRandomValueByData(encryptDifference1[i]);
		auto charact2 = characterizeRandomValueByData(encryptDifference2[i]);
		fout << i << "\t" << charact1.expected * 100 << "\t" << charact2.expected * 100 << std::endl;
	}
	
	fout.close();
}