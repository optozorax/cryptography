#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "encrypt.h"

int main() {
	TextReader texts(false);

	Frequency f;
	std::string text;
	for (int i = 0; i < texts.size(); i++) {
		texts.readNextText(text);
		f.count(text);
		std::cout << ".";
	}
	f.normalize();

	f.writeAsCode("frequency_code.cpp");
	f.write("frequency_table.txt");
	f.writeSorted("frequency_table_sorted.txt");
}