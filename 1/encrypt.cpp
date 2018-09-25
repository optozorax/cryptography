#include <fstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "encrypt.h"

//-----------------------------------------------------------------------------
Frequency::Frequency() {
	clear();
}

//-----------------------------------------------------------------------------
void Frequency::clear() {
	m_last = '!';
	f1.clear();
	f2.clear();
	f1.resize(alphabetSize, 0);
	f2.resize(alphabetSize, std::vector<double>(alphabetSize, 0));
	m_isNormalized = false;
	m_count = 0;
}

//-----------------------------------------------------------------------------
void Frequency::count(const std::string& text) {
	if (m_isNormalized)
		throw std::exception();

	char c;
	for (auto i : text) {
		i = toRussianLetters(i);
		if (i != '\0') {
			i -= rus_a;
			m_count++;
			f1[i]++;
			if (m_last != '!')
				f2[m_last][i]++;
			m_last = i;
		}	
	}
}

//-----------------------------------------------------------------------------
void Frequency::normalize() {
	if (m_isNormalized)
		throw std::exception();

	for (auto& i : f1)
		i /= m_count;

	for (auto& i : f2)
		for (auto& j : i)
			j /= m_count;

	m_isNormalized = true;
}

//-----------------------------------------------------------------------------
void Frequency::write(std::string fileName) const {
	if (!m_isNormalized)
		throw std::exception();

	std::ofstream fout(fileName);

	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	for (int i = 0; i < alphabetSize; ++i) {
		fout << char(rus_a + i) << "\t" << std::fixed << f1[i] << std::endl;
	}
	fout << std::endl;

	for (int i = 0; i < alphabetSize; ++i) {
		for (int j = 0; j < alphabetSize; ++j)
			fout << char(rus_a + i) << char(rus_a + j) << "\t" << std::fixed << f2[i][j] << std::endl;		
	}

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::writeSorted(std::string fileName) const {
	std::vector<Frequency::LetterAndNumber> sf1;
	std::vector<Frequency::TwoLetterAndNumber> sf2;
	getSortedF1(sf1);
	getSortedF1(sf2);
	
	std::ofstream fout(fileName);

	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);

	for (auto& i : sf1)
		fout << i.c << "\t" << std::fixed << i.f << std::endl;

	fout << std::endl;

	for (auto& i : sf2)
		fout << i.c1 << i.c2 << "\t" << std::fixed << i.f << std::endl;

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::writeAsCode(std::string fileName) const {
	if (!m_isNormalized)
		throw std::exception();

	std::ofstream fout(fileName);

	fout << "std::vector<double> f1 = {";
	int digits10 = std::numeric_limits<double>::digits10;
	fout.precision(digits10+1);
	for (auto& i : f1)
		fout << std::fixed << i << ", ";
	fout << "};" << std::endl;

	fout << "std::vector<std::vector<double>> f2 = {" << std::endl;
	for (auto& i : f2) {
		fout << "\t{";
		for (auto& j : i)
			fout << std::fixed << std::setw(digits10+3) << j << ", ";
		fout << "}," << std::endl;
	}
	fout << "};";

	fout.close();
}

//-----------------------------------------------------------------------------
void Frequency::load(std::string fileName) {
	clear();

	std::ifstream fin(fileName);

	char c;
	for (int i = 0; i < alphabetSize; ++i)
		fin >> c >> f1[i];

	for (int i = 0; i < alphabetSize; ++i)
		for (int j = 0; j < alphabetSize; ++j)
			fin >> c >> c >> f2[i][j];

	fin.close();

	m_isNormalized = true;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Frequency::countDifference(const Frequency& a) const {
	if (!m_isNormalized || !a.m_isNormalized)
		throw std::exception();

	std::pair<double, double> result = {};
	for (int i = 0; i < alphabetSize; ++i) {
		result.first += metric(f1[i], a.f1[i]);
		for (int j = 0; j < alphabetSize; ++j)
			result.second += metric(f2[i][j], a.f2[i][j]);
	}
	result.first /= alphabetSize;
	result.second /= alphabetSize * alphabetSize;

	return result;
}

//-----------------------------------------------------------------------------
std::pair<double, double> Frequency::countDifference(const Frequency& a, char symbol) const {
	if (!m_isNormalized || !a.m_isNormalized)
		throw std::exception();

	symbol -= rus_a;

	std::pair<double, double> result = {};
	result.first += metric(f1[symbol], a.f1[symbol]);
	for (int j = 0; j < alphabetSize; ++j) {
		result.second += metric(f2[symbol][j], a.f2[symbol][j]);
		if (j != 0)
			result.second += metric(f2[j][symbol], a.f2[j][symbol]);
	}
	result.second /= alphabetSize * 2 - 1;

	return result;
}

//-----------------------------------------------------------------------------
int64_t Frequency::getCount(void) const {
	return m_count;
}

//-----------------------------------------------------------------------------
void Frequency::getF1(std::vector<LetterAndNumber>& f) const {
	f.clear();
	for (int i = 0; i < f1.size(); ++i)
		f.push_back({char(rus_a + i), f1[i]});
}

//-----------------------------------------------------------------------------
void Frequency::getF2(std::vector<TwoLetterAndNumber>& f) const {
	f.clear();
	for (int i = 0; i < f2.size(); ++i)
		for (int j = 0; j < f2[i].size(); ++j)
			f.push_back({char(rus_a + i), char(rus_a + j), f2[i][j]});
}

//-----------------------------------------------------------------------------
void Frequency::getSortedF1(std::vector<LetterAndNumber>& sortedF1) const {
	getF1(sortedF1);
	std::sort(sortedF1.begin(), sortedF1.end(), [] (auto& a, auto& b) -> bool {
		return a.f > b.f;
	});
}

//-----------------------------------------------------------------------------
void Frequency::getSortedF1(std::vector<TwoLetterAndNumber>& sortedF2) const {
	getF2(sortedF2);
	std::sort(sortedF2.begin(), sortedF2.end(), [] (auto& a, auto& b) -> bool {
		return a.f > b.f;
	});
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Cipher::Cipher() {
}

//-----------------------------------------------------------------------------
void Cipher::mutate(void) {
	auto key = getKey();
	int a = intRandom(0, alphabetSize);
	int b = intRandom(0, alphabetSize);
	std::swap(key[a], key[b]);
	setKey(key);
}

//-----------------------------------------------------------------------------
void Cipher::generateHalfRandomKey(int swapCount) {
	setKey(getAlphabet());

	for (int i = 0; i < swapCount; ++i)
		mutate();
}

//-----------------------------------------------------------------------------
void Cipher::generateRandomKey(void) {
	std::string alphabet = getAlphabet();

	std::random_shuffle(alphabet.begin(), alphabet.end());

	setKey(alphabet);
}

//-----------------------------------------------------------------------------
void Cipher::setKey(std::string key) {
	if (key.size() == alphabetSize) {
		m_key = key;
		std::string reverseKey = key;
		for (int i = 0; i < alphabetSize; ++i)
			reverseKey[key[i] - rus_a] = i + rus_a;
		m_reverseKey = reverseKey;
	}
}

//-----------------------------------------------------------------------------
std::string Cipher::getKey(void) const {
	return m_key;
}

//-----------------------------------------------------------------------------
char Cipher::encrypt(char symbol) const {
	if (symbol >= rus_a && symbol <= rus_z)
		return m_key[symbol - rus_a];
	else
		return '\0';
}

//-----------------------------------------------------------------------------
std::string Cipher::encrypt(std::string text) const {
	for (auto& i : text)
		i = encrypt(i);
	return text;
}

//-----------------------------------------------------------------------------
char Cipher::decrypt(char symbol) const {
	if (symbol >= rus_a && symbol <= rus_z)
		return m_reverseKey[symbol - rus_a];
	else
		return '\0';
}

//-----------------------------------------------------------------------------
std::string Cipher::decrypt(std::string text) const {
	for (auto& i : text)
		i = decrypt(i);
	return text;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double random(void) {
	return std::rand() / double(RAND_MAX + 1);
}

//-----------------------------------------------------------------------------
int intRandom(int min, int max) {
	return min + random() * (max - min);
}

//-----------------------------------------------------------------------------
double metric(double a, double b) {
	if (a != 0 && b != 0)
		return fabs(a - b) / std::max(a, b);
	else
		return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string readFromFile(std::string fileName) {
	std::ifstream fin(fileName);
	std::string result;
	char temp;

	while (!fin.eof()) {
		fin >> temp;
		if (!fin.eof())
			result += temp;
	}
	 
	fin.close();

	return result;
}

//-----------------------------------------------------------------------------
void writeToFile(std::string fileName, const std::string& text) {
	std::ofstream fout(fileName);

	for (auto i : text)
		fout << i;
	 
	fout.close();
}

//-----------------------------------------------------------------------------
std::vector<std::string> readTexts(void) {
	std::vector<std::string> texts;
	for (int i = 0; i < 39; i++) {
		std::stringstream sout;
		sout << "text/1 (" << i + 1 << ").txt";
		auto text = readFromFile(sout.str());
		text = toRussianLetters(text);
		texts.push_back(text);
		std::cout << "\rTexts loading: " << (i * 100 / 39) << "%" << "         ";
	}
	std::cout << "\r" << "                                " << "\r";
	return texts;
}

//-----------------------------------------------------------------------------
char toRussianLetters(char c) {
	if (c == -72)
		return rus_a + 6;
	
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
std::string getRandomPart(const std::string& text, int size) {
	if (text.size() < size)
		return text;
	else {
		int pos = intRandom(0, text.size() - size);
		return text.substr(pos, size);
	}
}

//-----------------------------------------------------------------------------
std::string getAlphabet(void) {
	std::string alphabet;
	for (char i = rus_a; i <= rus_z; i++)
		alphabet.push_back(i);
	return alphabet;
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