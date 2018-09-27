/** ОПИСАНИЕ
		Эта программа подсчитывает частотность букв и сочетаний в русском языке. Для этого ей необходима папка с текстами. Выводит как таблицу частотности, которая потом используется в других программах, так и отсортированную таблицу частотности, чтобы посмотреть самые частые буквы и сочетания.
 */

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>

#include "encrypt.h"

int main() {
	TextReader texts(false);

	Frequency f;
	std::string text;
	for (int i = 0; i < texts.size(); i++) {
		texts.readNextText(text);
		f.count(text);

		// Выводим прогресс на экран
		std::cout << "\r" << std::setprecision(3) << std::setw(6) << double(i * 100)/texts.size() << "%";
	}
	f.normalize();

	f.write("frequency_table.txt");
	f.writeSorted("frequency_table_sorted.txt");
}