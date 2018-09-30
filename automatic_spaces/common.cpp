#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "common.h"

//-----------------------------------------------------------------------------
double random(void) {
	static std::mt19937 generator;
	static std::uniform_real_distribution<double> distribution(0, 1);
	return distribution(generator);
}

//-----------------------------------------------------------------------------
int intRandom(int min, int max) {
	int result = min + random() * (max - min);
	if (result == max)
		return min;
	else
		return result;
}

//-----------------------------------------------------------------------------
int getRandomCase(const std::vector<double>& cases) {
	double sum = 0;
	for (const auto& i : cases)
		sum += i;

	double rand = random();
	for (int i = 0; i < cases.size(); ++i) {
		if (rand < cases[i]/sum)
			return i;
		rand -= cases[i]/sum;
	}
	return 0;
}

//-----------------------------------------------------------------------------
void mutate(double& number, double min, double max) {
	if (intRandom(0, 2) == 0)
		number += random();
	else
		number -= random();

	if (number < min)
		number = min;

	if (number > max)
		number = max;
}

//-----------------------------------------------------------------------------
void mutate(int& number, int min, int max) {
	if (intRandom(0, 2) == 0)
		number++;
	else
		number--;

	if (number < min)
		number = min;

	if (number > max)
		number = max;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName) {
	std::ifstream fin(fileName);
	std::string result;
	result.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	fin.close();

	return result;
}

//-----------------------------------------------------------------------------
void writeToFile(std::string fileName, const std::string& text) {
	std::ofstream fout(fileName);
	fout << text;
	fout.close();
}

//-----------------------------------------------------------------------------
std::string getRandomPart(const std::string& text, int size) {
	if (text.size() < size)
		return text;
	else {
		int pos = intRandom(0, text.size() - size);
		return text.substr(pos, size);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
char toRussianLetters(char c) {
	if (c == rus_spare_e || c == rus_spare_E)
		return rus_a + 5;

	if (c >= rus_a && c <= rus_z)
		return c;
	else {
		if (c >= rus_A && c <= rus_Z)
			return rus_a + (c - rus_A);
		else
			return '\0';
	}
}

//-----------------------------------------------------------------------------
std::string toRussianLetters(std::string text) {
	std::string result;
	for (auto i : text) {
		i = toRussianLetters(i);
		if (i != '\0')
			result.push_back(i);
	}

	return result;
}

//-----------------------------------------------------------------------------
char toRussianLettersAndSpace(char c) {
	if (c == rus_space)
		return c;

	if (c == rus_spare_e || c == rus_spare_E)
		return rus_a + 5;

	if (c >= rus_a && c <= rus_z)
		return c;
	else {
		if (c >= rus_A && c <= rus_Z)
			return rus_a + (c - rus_A);
		else
			return '\0';
	}
}

//-----------------------------------------------------------------------------
std::string toRussianLettersAndSpace(std::string text) {
	std::string result;
	for (auto i : text) {
		i = toRussianLettersAndSpace(i);
		if (i != '\0' && i != rus_space)
			result.push_back(i);
		else
			if (result.empty())
				result.push_back(rus_space);
			else
				if (result.back() != rus_space)
					result.push_back(rus_space);
	}
	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TextReader::TextReader(bool isTestTexts) {
	if (!isTestTexts) {
		path = "text/0 (";
		count = 220;
	} else {
		path = "test_text/0 (";
		count = 8;
	}
	current = 0;
}

//-----------------------------------------------------------------------------
void TextReader::readNextText(std::string& text) {
	current++;

	std::stringstream sout;
	sout << path;
	sout << current << ").txt";

	text = readFromFile(sout.str());
}

//-----------------------------------------------------------------------------
int TextReader::size(void) const {
	return count;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RandomValueCharacterization characterizeRandomValueByData(const std::vector<double>& data) {
	RandomValueCharacterization result;
	result.min = data[0];
	result.max = data[0];
	result.expected = 0;
	result.dispersion = 0;

	for (auto& i : data) {
		if (i < result.min)
			result.min = i;
		if (i > result.max)
			result.max = i;
		result.expected += i;
	}
	result.expected /= data.size();

	for (auto& i : data) 
		result.dispersion += (i - result.expected) * (i - result.expected);
	result.dispersion /= data.size();
	result.dispersion = sqrt(result.dispersion);

	return result;
}

//-----------------------------------------------------------------------------
std::ofstream& operator<<(std::ofstream& out, const RandomValueCharacterization& c) {
	out << std::fixed << std::setprecision(4);
	out << "expected:   " << c.expected   * 100 << "%" << std::endl;
	out << "dispersion: " << c.dispersion * 100 << "%" << std::endl;
	out << "min:        " << c.min        * 100 << "%" << std::endl;
	out << "max:        " << c.max        * 100 << "%" << std::endl;

	return out;
}