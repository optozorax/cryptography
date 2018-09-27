/** ОПИСАНИЕ
		Эта программа определяет записывает в файл буквы частотность всех двубуквенных и трехбуквенный сочетаний с ней, отсортированных по убыванию. Непосредственно расшифровки это не касается. Просто пусть будет, пригодится для оптимизации клавиатурной раскладки.
 */

#include <fstream>
#include "encrypt.h"

int main() {
	Frequency f;
	f.load("frequency_table.txt");

	std::vector<SymbolFrequency1> sf1;
	std::vector<SymbolFrequency2> sf2;
	std::vector<SymbolFrequency3> sf3;
	f.getSortedF1(sf1);
	f.getSortedF2(sf2);
	f.getSortedF3(sf3);
	
	for (int i = 0; i < alphabetSize; ++i) {
		char letter = rus_a + i;

		std::vector<SymbolFrequency1> slf1;
		std::vector<SymbolFrequency2> slf2;
		std::vector<SymbolFrequency3> slf3;

		for (auto& i : sf1)
			if (i.c1 == letter)
				slf1.push_back(i);
		for (auto& i : sf2)
			if (i.c1 == letter || i.c2 == letter)
				slf2.push_back(i);
		for (auto& i : sf3)
			if (i.c1 == letter || i.c2 == letter || i.c3 == letter)
				slf3.push_back(i);

		std::string name = "letters/";
		name += letter;
		name += ".txt";

		std::ofstream fout(name);

		for (auto& i : slf1)
			fout << i << std::endl;
		fout << std::endl;
		for (auto& i : slf2)
			fout << i << std::endl;
		fout << std::endl;
		for (auto& i : slf3)
			fout << i << std::endl;

		fout.close();	
	}
}