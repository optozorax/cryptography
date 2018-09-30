#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>

#include "common.h"
#include "space_stats.h"

int main() {
	TextReader texts(false);

	SpaceStatistics s;

	std::string text;
	for (int i = 0; i < texts.size(); i++) {
		texts.readNextText(text);
		s.count(text);

		// Выводим прогресс на экран
		std::cout << "\r" << std::setprecision(3) << std::setw(6) << double(i * 100)/texts.size() << "%";
	}
	s.normalize();

	s.save("frequency_space_table.txt");
}