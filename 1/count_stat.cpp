#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "encrypt.h"

int main() {
	Frequency f;

	std::vector<std::string> texts = readTexts();

	for (auto& i : texts)
		f.count(i);
	f.normalize();

	f.writeAsCode("frequency_code.cpp");
	f.write("frequency_table.txt");
	f.writeSorted("frequency_table_sorted.txt");
}